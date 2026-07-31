const std = @import("std");
const scheduler = @import("../task/scheduler.zig");
const TNode = @import("../fs/tnode.zig");
const Errno = @import("../errno.zig").Errno;

pub const Id = 31;

/// Walks up to the root collecting names, so the path comes out backwards and
/// is turned around at the end rather than built with repeated shifting.
pub fn do(buffer: [*]u8, size: usize) !usize {
    if (size == 0)
        return Errno.EINVAL;

    const task = scheduler.get_current_task();
    var reversed: [4096]u8 = undefined;
    var length: usize = 0;

    var node: *TNode = task.cwd;
    while (node != task.root and node != node.parent) : (node = node.parent) {
        if (length + node.name.len + 1 > reversed.len)
            return Errno.ENAMETOOLONG;
        var i = node.name.len;
        while (i > 0) : (i -= 1) {
            reversed[length] = node.name[i - 1];
            length += 1;
        }
        reversed[length] = '/';
        length += 1;
    }

    // The root itself has no name, and the loop above wrote nothing for it.
    if (length == 0) {
        if (size < 2)
            return Errno.ERANGE;
        buffer[0] = '/';
        buffer[1] = 0;
        return 2;
    }

    if (length + 1 > size)
        return Errno.ERANGE;
    for (0..length) |i|
        buffer[i] = reversed[length - 1 - i];
    buffer[length] = 0;
    return length + 1;
}
