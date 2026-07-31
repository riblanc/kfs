const std = @import("std");
const vfs = @import("../fs/vfs.zig");

pub const Id = 33;

/// Only existence is checked. There is no notion of a calling user yet, so
/// answering on read, write or execute would be an invention.
pub fn do(path: [*:0]const u8, _: u32) !void {
    const tnode = try vfs.resolve(std.mem.sliceTo(path, 0));
    tnode.release();
}
