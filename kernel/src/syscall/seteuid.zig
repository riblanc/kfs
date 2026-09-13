const scheduler = @import("../task/scheduler.zig");

pub const Id = 60;

/// Set the effective user id, POSIX seteuid.
///
/// An unprivileged task may only pick one of the three uids it already holds.
pub fn do(uid: u32) !void {
    const task = scheduler.get_current_task();

    if (!task.is_privileged() and
        uid != task.uid and uid != task.euid and uid != task.suid)
        return error.EPERM;

    task.euid = uid;
}
