const std = @import("std");
const tty = @import("../device/tty/tty.zig");
const scheduler = @import("../task/scheduler.zig");
const TaskDescriptor = @import("../task/task.zig").TaskDescriptor;
pub const Id = 2;

/// Write to the tty in red. Going through escape sequences rather than the
/// console state keeps this working on a serial line too.
fn write_stderr(data: []const u8) usize {
    const colors = @import("colors");
    const writer = tty.get_writer();
    _ = writer.write(colors.red) catch {};
    const written = writer.write(data) catch unreachable;
    _ = writer.write(colors.reset) catch {};
    return written;
}

pub fn do(fd: TaskDescriptor.Fd, buf: [*]align(1) const u8, len: usize) !usize {
    // todo: implement tty char device
    switch (fd) {
        1 => return tty.get_writer().write(buf[0..len]) catch unreachable,
        2 => return write_stderr(buf[0..len]),
        else => {},
    }

    const file = scheduler.get_current_task().get_file(fd) orelse return error.EBADF;

    return file.write(buf[0..len]);
}
