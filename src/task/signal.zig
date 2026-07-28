const std = @import("std");
const task = @import("task.zig");
const TaskDescriptor = task.TaskDescriptor;
const scheduler = @import("scheduler.zig");
const task_set = @import("task_set.zig");
const paging = @import("../memory/paging.zig");
const Monostate = @import("../misc/monostate.zig").Monostate;
const Cache = @import("../memory/object_allocators/slab/cache.zig").Cache;
const globalCache = &@import("../memory.zig").globalCache;
const Errno = @import("../errno.zig").Errno;
const Mutex = @import("semaphore.zig").Mutex;
const logger = std.log.scoped(.signal);

pub const DefaultAction = enum {
    Ignore,
    Terminate,
    Stop,
    Continue,
};

pub const Handler = *allowzero const fn (u32) callconv(.c) void;
pub const SigactionHandler = *allowzero const fn (u32, *siginfo_t, *void) callconv(.c) void;
pub const SIG_DFL: Handler = @ptrFromInt(0);
pub const SIG_IGN: Handler = @ptrFromInt(1);

/// Numbers as abi-bits/signal.h assigns them, which is the set i386 Linux uses.
/// POSIX names signals but does not number them, so the only requirement is
/// that the libc and the kernel agree, and the libc headers come from mlibc.
///
/// These were the System V numbers before, which Solaris and Linux on MIPS
/// still use. SIGEMT belongs to that set and has no number here; SIGSTKFLT
/// takes the slot the System V table gave to SIGUSR1.
pub const Id = enum(u32) {
    SIGHUP = 1,
    SIGINT = 2,
    SIGQUIT = 3,
    SIGILL = 4,
    SIGTRAP = 5,
    SIGABRT = 6,
    SIGBUS = 7,
    SIGFPE = 8,
    SIGKILL = 9,
    SIGUSR1 = 10,
    SIGSEGV = 11,
    SIGUSR2 = 12,
    SIGPIPE = 13,
    SIGALRM = 14,
    SIGTERM = 15,
    SIGSTKFLT = 16,
    SIGCHLD = 17,
    SIGCONT = 18,
    SIGSTOP = 19,
    SIGTSTP = 20,
    SIGTTIN = 21,
    SIGTTOU = 22,
    SIGURG = 23,
    SIGXCPU = 24,
    SIGXFSZ = 25,
    SIGVTALRM = 26,
    SIGPROF = 27,
    SIGWINCH = 28,
    SIGPOLL = 29,
    SIGPWR = 30,
    SIGSYS = 31,

    /// The same signal under its other name.
    pub const SIGIO = Id.SIGPOLL;
};

pub const Code = enum(u32) {
    SI_USER,
    SEGV_ACCERR,
    SEGV_MAPERR,
};

pub const siginfo_t = extern struct {
    si_signo: Signo = Signo.invalid,
    si_code: Code = undefined,
    si_errno: u32 = undefined,
    si_pid: TaskDescriptor.Pid = undefined, // todo pid type
    // si_uid
    si_addr: paging.VirtualPtr = undefined,
    si_status: u32 = undefined,
    // si_value : sigval
    pub const Signo = packed union {
        valid: Id,
        null: Monostate(u32, 0),
        pub const invalid = @This(){ .null = .{} };
        pub fn make(id: Id) @This() {
            return .{ .valid = id };
        }
        pub fn unwrap(self: @This()) Id {
            return if (@as(u32, @bitCast(self)) == 0) @panic("invalid signo") else self.valid;
        }
        pub fn safeUnwrap(self: @This()) ?Id {
            return if (@as(u32, @bitCast(self)) == 0) null else self.valid;
        }
    };
};

pub const SigSet = u32;

pub const Sigaction = extern struct {
    sa_handler: Handler = SIG_DFL,
    sa_sigaction: SigactionHandler = undefined,
    sa_mask: SigSet = 0,
    sa_flags: packed struct(u32) {
        SA_NOCLDSTOP: bool = false, // todo: implement this option
        // SA_ONSTACK, : bool = false,
        SA_RESETHAND: bool = false, // todo: implement this option
        SA_RESTART: bool = false, // todo: implement this option
        SA_SIGINFO: bool = false,
        // SA_NOCLDWAIT : bool = false,
        SA_NODEFER: bool = false, // todo
        // SS_ONSTACK : bool = false,
        // SS_DISABLE : bool = false,
        // MINSIGSTKSZ : bool = false,
        // SIGSTKSZ : bool = false,
        _unused: u27 = 0,
    } = .{},
};

pub const SignalQueue = struct {
    default_handler: DefaultAction,
    action: Sigaction,
    queue: QueueType = .{},
    ignorable: bool = true,

    const SignalNode = struct {
        node: std.DoublyLinkedList.Node,
        data: siginfo_t,
    };
    const QueueType = std.DoublyLinkedList;
    pub var cache: *Cache = undefined;
    const Self = @This();

    pub fn init(default_handler: DefaultAction, ignorable: bool) Self {
        return Self{
            .default_handler = default_handler,
            .action = .{ .sa_handler = SIG_DFL },
            .ignorable = ignorable,
        };
    }

    pub fn init_cache() !void {
        cache = try globalCache.create(
            "signal node",
            @import("../memory.zig").virtually_contiguous_page_allocator.page_allocator(),
            @sizeOf(SignalNode),
            @alignOf(SignalNode),
            3,
        );
    }

    fn is_ignored(self: Self) bool {
        return !self.action.sa_flags.SA_SIGINFO and
            (self.action.sa_handler == SIG_IGN or
                (self.action.sa_handler == SIG_DFL and self.default_handler == .Ignore));
    }

    pub fn queue_signal(self: *Self, signal: siginfo_t) void {
        if (self.is_ignored()) {
            return;
        }
        const signal_node = cache.allocator().create(SignalNode) catch @panic("out of space");
        signal_node.data = signal;
        self.queue.append(&signal_node.node);
    }

    pub fn pop(self: *Self) ?siginfo_t {
        if (self.queue.popFirst()) |node| {
            const signal_node: *SignalNode = @fieldParentPtr("node", node);
            const ret = signal_node.data;
            cache.allocator().destroy(signal_node);
            return ret;
        } else return null;
    }

    pub fn set_action(self: *Self, action: Sigaction) !void {
        if (!self.ignorable)
            return Errno.EINVAL;
        self.action = action;
        if (self.is_ignored()) {
            while (self.queue.len() != 0) {
                _ = self.queue.popFirst();
            }
        }
    }
};

pub const SignalManager = struct {
    queues: [32]SignalQueue = undefined,
    pending: SigSet = 0,
    mutex: Mutex = .{},
    const Self = @This();

    const non_maskable: SigSet = (@as(SigSet, 1) << @intFromEnum(Id.SIGKILL)) |
        (@as(SigSet, 1) << @intFromEnum(Id.SIGSTOP));

    /// What a signal does with no handler installed. Exhaustive on purpose: a
    /// new Id has to be given an action here before it compiles.
    fn default_action(id: Id) DefaultAction {
        return switch (id) {
            .SIGCHLD, .SIGURG, .SIGWINCH => .Ignore,
            .SIGCONT => .Continue,
            .SIGSTOP, .SIGTSTP, .SIGTTIN, .SIGTTOU => .Stop,
            .SIGABRT,
            .SIGALRM,
            .SIGBUS,
            .SIGFPE,
            .SIGHUP,
            .SIGILL,
            .SIGINT,
            .SIGKILL,
            .SIGPIPE,
            .SIGPOLL,
            .SIGPROF,
            .SIGPWR,
            .SIGQUIT,
            .SIGSEGV,
            .SIGSTKFLT,
            .SIGSYS,
            .SIGTERM,
            .SIGTRAP,
            .SIGUSR1,
            .SIGUSR2,
            .SIGVTALRM,
            .SIGXCPU,
            .SIGXFSZ,
            => .Terminate,
        };
    }

    fn is_ignorable(id: Id) bool {
        return switch (id) {
            .SIGKILL, .SIGSTOP => false,
            else => true,
        };
    }

    pub fn init() Self {
        // Slot 0 is not a signal and is never indexed, but leaving it undefined
        // is what let the missing entries above go unnoticed.
        var self = Self{ .queues = @splat(SignalQueue.init(.Terminate, true)) };
        inline for (@typeInfo(Id).@"enum".fields) |field| {
            const id: Id = @enumFromInt(field.value);
            self.queues[field.value] = SignalQueue.init(default_action(id), is_ignorable(id));
        }
        return self;
    }

    pub fn change_action(self: *Self, id: Id, action: Sigaction) !void {
        self.mutex.acquire();
        defer self.mutex.release();

        return self.queues[@intFromEnum(id)].set_action(action);
    }

    pub fn get_action(self: Self, id: Id) Sigaction {
        return self.queues[@intFromEnum(id)].action;
    }

    pub fn get_defaultAction(self: Self, id: Id) DefaultAction {
        return self.queues[@intFromEnum(id)].default_handler;
    }

    pub fn queue_signal(self: *Self, signal: siginfo_t) void {
        self.mutex.acquire();
        defer self.mutex.release();

        const index: u32 = @intFromEnum(signal.si_signo.unwrap());
        if (index > self.queues.len) {
            @panic("todo");
        }
        self.queues[index].queue_signal(signal);
        if (self.queues[index].queue.len() != 0) { // todo: there may be a better way to do this
            self.pending |= @as(SigSet, 1) << @as(u5, @intCast(index));
        }
    }

    pub fn get_pending_signal(self: *Self, mask: SigSet) ?siginfo_t {
        self.mutex.acquire();
        defer self.mutex.release();

        const real_mask: SigSet = mask & ~non_maskable;
        if ((self.pending & ~real_mask) != 0) {
            const signo = @ctz(self.pending & ~real_mask);
            const q = &self.queues[signo];
            if (q.pop()) |s| {
                if (q.queue.len() == 0) {
                    self.pending ^= @as(SigSet, 1) << @intCast(signo);
                }
                return s;
            } else unreachable;
        }
        return null;
    }

    pub fn has_pending(self: Self) bool {
        return self.pending != 0;
    }
};
