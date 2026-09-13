const scheduler = @import("../task/scheduler.zig");

pub const Id = 61;

/// Set the effective group id, POSIX setegid.
///
/// An unprivileged task may only pick one of the three gids it already holds.
pub fn do(gid: u32) !void {
    const task = scheduler.get_current_task();

    if (!task.is_privileged() and
        gid != task.gid and gid != task.egid and gid != task.sgid)
        return error.EPERM;

    task.egid = gid;
}
