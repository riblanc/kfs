const std = @import("std");
pub const Id = 54;
const Errno = @import("../errno.zig").Errno;
const scheduler = @import("../task/scheduler.zig");
const TaskDescriptor = @import("../task/task.zig").TaskDescriptor;
const FileSet = @import("../task/file_set.zig");
const vfs = @import("../fs/vfs.zig");

/// The commands POSIX fcntl takes. The numbers are the ones mlibc gives
/// F_DUPFD and the rest.
pub const Cmd = enum(usize) {
    DupFd = 0,
    GetFd = 1,
    SetFd = 2,
    GetFl = 3,
    SetFl = 4,
    _,
};

/// A descriptor carries no flags of its own yet: FD_CLOEXEC is accepted and
/// dropped, and the file status flags read back as none.
pub fn do(fd : FileSet.Fd, cmd : Cmd, arg : usize) !usize {
    const task = scheduler.get_current_task();
    switch (cmd) {
        .DupFd => {
            const file = try task.files.get(fd);
            const ret : usize = @intCast(try task.files.add(file.get_ref(), @intCast(arg)));
            std.log.debug("cmd: {}, arg: {}, ret: {}", .{cmd, arg, ret});
            return ret;
        },
        .GetFd, .GetFl => {
            _ = try task.files.get(fd);
            return 0;
        },
        .SetFd, .SetFl => {
            _ = try task.files.get(fd);
            return 0;
        },
        _ => return Errno.EINVAL,
    }
}
