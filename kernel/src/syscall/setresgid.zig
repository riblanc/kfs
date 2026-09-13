const scheduler = @import("../task/scheduler.zig");
const TaskDescriptor = @import("../task/task.zig").TaskDescriptor;

pub const Id = 63;

/// Set the real, effective and saved group ids, POSIX setresgid.
/// TaskDescriptor.ID_UNCHANGED leaves an id as it is.
///
/// An unprivileged task may only shuffle the three gids it already holds, and
/// either every id is written or none is.
pub fn do(rgid: u32, egid: u32, sgid: u32) !void {
    const task = scheduler.get_current_task();

    if (!task.is_privileged()) {
        for ([_]u32{ rgid, egid, sgid }) |id| {
            if (id == TaskDescriptor.ID_UNCHANGED) continue;
            if (id != task.gid and id != task.egid and id != task.sgid)
                return error.EPERM;
        }
    }

    if (rgid != TaskDescriptor.ID_UNCHANGED) task.gid = rgid;
    if (egid != TaskDescriptor.ID_UNCHANGED) task.egid = egid;
    if (sgid != TaskDescriptor.ID_UNCHANGED) task.sgid = sgid;
}
