const scheduler = @import("../task/scheduler.zig");

pub const Id = 59;

/// Set the group id, POSIX setgid.
///
/// A privileged task sets the real, effective and saved gids at once. An
/// unprivileged one may only move its effective gid to its real or saved gid.
pub fn do(gid: u32) !void {
    const task = scheduler.get_current_task();

    if (task.is_privileged()) {
        task.gid = gid;
        task.egid = gid;
        task.sgid = gid;
        return;
    }

    if (gid != task.gid and gid != task.sgid) return error.EPERM;
    task.egid = gid;
}
