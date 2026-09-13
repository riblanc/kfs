const scheduler = @import("../task/scheduler.zig");

pub const Id = 64;

/// Read the real, effective and saved user ids, POSIX getresuid.
pub fn do(ruid: *align(1) u32, euid: *align(1) u32, suid: *align(1) u32) !void {
    const task = scheduler.get_current_task();

    ruid.* = task.uid;
    euid.* = task.euid;
    suid.* = task.suid;
}
