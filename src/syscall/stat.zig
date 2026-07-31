const std = @import("std");
const vfs = @import("../fs/vfs.zig");
const abi = @import("../fs/abi.zig");

pub const Id = 32;

pub fn do(path: [*:0]const u8, statbuf: *abi.Stat) !void {
    const tnode = try vfs.resolve(std.mem.sliceTo(path, 0));
    defer tnode.release();
    statbuf.* = abi.Stat.from_inode(tnode.inode);
}
