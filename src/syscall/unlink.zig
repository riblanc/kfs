const std = @import("std");
const vfs = @import("../fs/vfs.zig");
const Errno = @import("../errno.zig").Errno;

pub const Id = 34;

pub fn do(path: [*:0]const u8) !void {
    const slice = std.mem.sliceTo(path, 0);
    // Removing a name means changing the directory that holds it, so the
    // parent is what has to be resolved.
    const dirname = std.fs.path.dirnamePosix(slice) orelse
        if (std.fs.path.isAbsolute(slice)) return Errno.EBUSY else ".";
    const tnode = try vfs.resolve(dirname);
    defer tnode.release();
    return tnode.inode.unlink(std.fs.path.basenamePosix(slice));
}
