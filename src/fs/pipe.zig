// Anonymous pipes.
//
// A pipe has no name and no filesystem behind it, so it carries its own inode
// and a superblock that exists only to free it. The two ends are separate File
// objects over that one inode: each counts its own handles, which is what tells
// a reader that the last writer is gone and a writer that nobody is listening.
const std = @import("std");

const Errno = @import("../errno.zig").Errno;
const File = @import("file.zig");
const Inode = @import("inode.zig");
const SuperBlock = @import("superblock.zig");
const scheduler = @import("../task/scheduler.zig");
const signal = @import("../task/signal.zig");
const wait_queue = @import("../task/wait_queue.zig");

const allocator = @import("../memory.zig").smallAlloc.allocator();

/// POSIX asks for at least 4096 bytes to be written atomically.
pub const capacity = 4096;

const Self = @This();

inode: Inode,
buffer: [capacity]u8 = undefined,
head: usize = 0,
tail: usize = 0,
count: usize = 0,

/// Open handles on each end. A pipe is freed once both reach zero.
readers: usize = 1,
writers: usize = 1,

readable: wait_queue.WaitQueue(.{ .predicate = can_read }) = .{},
writable: wait_queue.WaitQueue(.{ .predicate = can_write }) = .{},

fn can_read(_: *void, data: ?*void) bool {
    const self: *Self = @ptrCast(@alignCast(data.?));
    // No writer left counts as ready: the reader has to wake up to see the end
    // of the pipe rather than sleep on it forever.
    return self.count != 0 or self.writers == 0;
}

fn can_write(_: *void, data: ?*void) bool {
    const self: *Self = @ptrCast(@alignCast(data.?));
    return self.count != capacity or self.readers == 0;
}

fn from_inode(inode: *Inode) *Self {
    return @fieldParentPtr("inode", inode);
}

fn from_file(file: *File) *Self {
    return from_inode(file.inode);
}

// The ring

fn take(self: *Self, out: []u8) usize {
    const size = @min(out.len, self.count);
    for (out[0..size]) |*byte| {
        byte.* = self.buffer[self.tail];
        self.tail = (self.tail + 1) % capacity;
    }
    self.count -= size;
    return size;
}

fn put(self: *Self, data: []const u8) usize {
    const size = @min(data.len, capacity - self.count);
    for (data[0..size]) |byte| {
        self.buffer[self.head] = byte;
        self.head = (self.head + 1) % capacity;
    }
    self.count += size;
    return size;
}

// The two ends

fn read(file: *File, buffer: []u8) File.Error.read!usize {
    const self = from_file(file);
    while (self.count == 0) {
        if (self.writers == 0)
            return 0;
        self.readable.block(scheduler.get_current_task(), @ptrCast(self)) catch
            return error.EINTR;
    }
    const size = self.take(buffer);
    self.writable.try_unblock();
    return size;
}

fn write(file: *File, data: []const u8) File.Error.write!usize {
    const self = from_file(file);
    var written: usize = 0;
    while (written < data.len) {
        while (self.count == capacity) {
            if (self.readers == 0)
                break;
            self.writable.block(scheduler.get_current_task(), @ptrCast(self)) catch
                return if (written != 0) written else error.EINTR;
        }
        // Writing into a pipe nobody holds open is the writer's problem to
        // hear about, so it is told twice: by signal and by return value.
        if (self.readers == 0) {
            scheduler.get_current_task().send_signal(
                signal.siginfo_t.init(.{ .kernel = .SIGPIPE }),
            );
            return if (written != 0) written else error.EPIPE;
        }
        written += self.put(data[written..]);
        self.readable.try_unblock();
    }
    return written;
}

fn close_read_end(file: *File) File.Error.close!void {
    const self = from_file(file);
    self.readers -= 1;
    // Wake the writers so they find out there is no one left to read them.
    if (self.readers == 0)
        self.writable.unblock_all();
    file.inode.release();
}

fn close_write_end(file: *File) File.Error.close!void {
    const self = from_file(file);
    self.writers -= 1;
    // Wake the readers so they see the end of the pipe instead of waiting.
    if (self.writers == 0)
        self.readable.unblock_all();
    file.inode.release();
}

fn not_seekable(_: *File, _: File.Off, _: File.Seek) File.Error.seek!File.Off {
    return error.ESPIPE;
}

fn not_a_directory(_: *File, _: *File.DirEnt) File.Error.readdir!bool {
    return error.EPERM;
}

const read_end_vtable: File.VTable = .{
    .close = &close_read_end,
    .read = &read,
    .write = &write_to_read_end,
    .readdir = &not_a_directory,
    .seek = &not_seekable,
};

const write_end_vtable: File.VTable = .{
    .close = &close_write_end,
    .read = &read_from_write_end,
    .write = &write,
    .readdir = &not_a_directory,
    .seek = &not_seekable,
};

fn write_to_read_end(_: *File, _: []const u8) File.Error.write!usize {
    return error.EBADF;
}

fn read_from_write_end(_: *File, _: []u8) File.Error.read!usize {
    return error.EBADF;
}

// The inode, and the superblock that exists only to free it

fn flush(_: *Inode) Inode.Error.flush!void {}

fn truncate(_: *Inode, _: u64) Inode.Error.truncate!void {
    return error.EPERM;
}

fn link(_: *Inode, _: []const u8, _: *Inode) Inode.Error.link!void {
    return error.EPERM;
}

fn unlink(_: *Inode, _: []const u8) Inode.Error.unlink!void {
    return error.EPERM;
}

/// A pipe is only ever reached through the two File objects create hands out,
/// which carry their own read and write. ESPIPE would say it better, but these
/// cannot be called, and widening the error set for them would be pretending
/// otherwise.
fn pread(_: *Inode, _: u64, _: []u8) Inode.Error.pread!usize {
    return error.EPERM;
}

fn pwrite(_: *Inode, _: u64, _: []const u8) Inode.Error.pwrite!usize {
    return error.EPERM;
}

fn preaddir(_: *Inode, _: u64, _: *Inode.DirEnt) Inode.Error.preaddir!usize {
    return error.EPERM;
}

const inode_vtable: Inode.VTable = .{
    .flush = &flush,
    .truncate = &truncate,
    .link = &link,
    .unlink = &unlink,
    .pread = &pread,
    .pwrite = &pwrite,
    .preaddir = &preaddir,
};

fn release_inode(_: *SuperBlock, inode: *Inode) SuperBlock.Error.release_inode!void {
    allocator.destroy(from_inode(inode));
}

fn load_inode(_: *SuperBlock, _: Inode.Ino) SuperBlock.Error.load_inode!*Inode {
    // A pipe is never looked up: it is only ever handed out by create.
    return error.ENODEV;
}

fn get_root(_: *SuperBlock) SuperBlock.Error.get_root!*Inode {
    return error.ENODEV;
}

fn create_inode(
    _: *SuperBlock,
    _: Inode.Uid,
    _: Inode.Gid,
    _: Inode.Mode,
    _: SuperBlock.TypeSpecificParams,
) SuperBlock.Error.create_inode!*Inode {
    return error.EPERM;
}

const superblock_vtable: SuperBlock.VTable = .{
    .load_inode = &load_inode,
    .release_inode = &release_inode,
    .get_root = &get_root,
    .create_inode = &create_inode,
};

/// Never mounted, and it holds no inode cache: pipes are reached through the
/// descriptors create returns, so nothing looks one up by number.
var superblock: SuperBlock = undefined;
var next_ino: Inode.Ino = 1;

pub fn init() void {
    superblock = .{
        .block_size = 1,
        .fragment_size = 1,
        .blocks = 0,
        .free_blocks = 0,
        .reserved_blocks = 0,
        .files = 0,
        .free_files = 0,
        .reserved_files = 0,
        .fsid = null,
        .flags = .{ .read_only = false, .no_suid = true },
        .max_name = 0,
        .uuid = null,
        .partition = null,
        .vtable = &superblock_vtable,
        .cache = SuperBlock.InodeCache.init(allocator),
    };
}

/// A fresh pipe and the two ends onto it. The caller owns both files and has to
/// close each one.
pub fn create() !struct { *File, *File } {
    const self = allocator.create(Self) catch return Errno.ENOMEM;
    errdefer allocator.destroy(self);

    self.* = .{
        .inode = .{
            .superblock = &superblock,
            .ino = next_ino,
            .hard_links = 0,
            .size = 0,
            .uid = 0,
            .gid = 0,
            .mode = .{ .type = .Fifo },
            .type_specific = .{ .Fifo = {} },
            // One for each end, so the inode outlives whichever closes first.
            .refs = 2,
            .vtable = &inode_vtable,
        },
    };
    next_ino += 1;

    const read_end = try File.create();
    errdefer File.destroy(read_end);
    const write_end = try File.create();

    read_end.* = .{ .inode = &self.inode, .refs = 1, .vtable = &read_end_vtable };
    write_end.* = .{ .inode = &self.inode, .refs = 1, .vtable = &write_end_vtable };
    return .{ read_end, write_end };
}
