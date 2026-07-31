const scheduler = @import("../task/scheduler.zig");
const TaskDescriptor = @import("../task/task.zig").TaskDescriptor;
const Errno = @import("../errno.zig").Errno;
const pipe = @import("../fs/pipe.zig");

pub const Id = 29;

pub fn do(fds: *[2]TaskDescriptor.Fd) !void {
    const current = scheduler.get_current_task();
    const read_end, const write_end = try pipe.create();
    errdefer {
        read_end.close() catch {};
        write_end.close() catch {};
    }

    const read_fd = current.add_file(read_end) orelse return Errno.EMFILE;
    errdefer current.remove_file(read_fd);
    const write_fd = current.add_file(write_end) orelse return Errno.EMFILE;

    fds[0] = read_fd;
    fds[1] = write_fd;
}
