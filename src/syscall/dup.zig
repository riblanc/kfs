const scheduler = @import("../task/scheduler.zig");
const TaskDescriptor = @import("../task/task.zig").TaskDescriptor;
const Errno = @import("../errno.zig").Errno;

pub const Id = 27;

pub fn do(fd: TaskDescriptor.Fd) !TaskDescriptor.Fd {
    const current = scheduler.get_current_task();
    const file = current.get_file(fd) orelse return Errno.EBADF;
    // Both descriptors name the same open file, position included, so the
    // handle is shared rather than the inode reopened.
    return current.add_file(file.get_ref()) orelse {
        file.close() catch {};
        return Errno.EMFILE;
    };
}
