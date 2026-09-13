const scheduler = @import("../task/scheduler.zig");
const FileSet = @import("../task/file_set.zig");
const stat = @import("stat.zig");

pub const Id = 68;

/// Describe an open file, POSIX fstat.
pub fn do(fd: FileSet.Fd, dst: *stat.Stat) !void {
    const file = try scheduler.get_current_task().files.get(fd);
    dst.* = stat.of(file.inode);
}
