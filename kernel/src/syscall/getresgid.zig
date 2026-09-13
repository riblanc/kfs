const scheduler = @import("../task/scheduler.zig");

pub const Id = 65;

/// Read the real, effective and saved group ids, POSIX getresgid.
pub fn do(rgid: *align(1) u32, egid: *align(1) u32, sgid: *align(1) u32) !void {
    const task = scheduler.get_current_task();

    rgid.* = task.gid;
    egid.* = task.egid;
    sgid.* = task.sgid;
}
