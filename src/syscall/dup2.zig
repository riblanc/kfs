const scheduler = @import("../task/scheduler.zig");
const TaskDescriptor = @import("../task/task.zig").TaskDescriptor;
const Errno = @import("../errno.zig").Errno;

pub const Id = 28;

pub fn do(oldfd: TaskDescriptor.Fd, newfd: TaskDescriptor.Fd) !TaskDescriptor.Fd {
    const current = scheduler.get_current_task();
    const file = current.get_file(oldfd) orelse return Errno.EBADF;
    if (newfd < 0 or newfd >= current.files.len)
        return Errno.EBADF;

    // Asking for the descriptor it already is changes nothing, and must not
    // close it on the way.
    if (oldfd == newfd)
        return newfd;

    if (current.get_file(newfd)) |previous| {
        previous.close() catch {};
        current.remove_file(newfd);
    }
    current.files[@intCast(newfd)] = file.get_ref();
    return newfd;
}
