const scheduler = @import("../task/scheduler.zig");
const signal = @import("../task/signal.zig");

pub const Id = 66;

/// How a new set is merged into the blocked set, POSIX sigprocmask. The values
/// are the ones mlibc gives SIG_BLOCK, SIG_UNBLOCK and SIG_SETMASK.
pub const How = enum(u32) {
    BLOCK = 0,
    UNBLOCK = 1,
    SETMASK = 2,
    _,
};

/// Read and change the signals the caller blocks, POSIX sigprocmask.
///
/// SIGKILL and SIGSTOP are left out of the mask by the manager, so a task
/// cannot block them by asking for them here.
pub fn do(how: How, set: ?*const signal.SigSet, oldset: ?*signal.SigSet) !void {
    const task = scheduler.get_current_task();

    if (oldset) |out| out.* = task.ucontext.uc_sigmask;

    const wanted = (set orelse return).*;
    task.ucontext.uc_sigmask = switch (how) {
        .BLOCK => task.ucontext.uc_sigmask | wanted,
        .UNBLOCK => task.ucontext.uc_sigmask & ~wanted,
        .SETMASK => wanted,
        _ => return error.EINVAL,
    };
}
