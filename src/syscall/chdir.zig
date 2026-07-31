const std = @import("std");
const scheduler = @import("../task/scheduler.zig");
const vfs = @import("../fs/vfs.zig");
const Errno = @import("../errno.zig").Errno;

pub const Id = 30;

pub fn do(path: [*:0]const u8) !void {
    const tnode = try vfs.resolve(std.mem.sliceTo(path, 0));
    defer tnode.release();
    if (tnode.inode.mode.type != .Directory)
        return Errno.ENOTDIR;
    scheduler.get_current_task().chdir(tnode);
}
