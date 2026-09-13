const scheduler = @import("../task/scheduler.zig");
const TaskDescriptor = @import("../task/task.zig").TaskDescriptor;

pub const Id = 62;

/// Set the real, effective and saved user ids, POSIX setresuid.
/// TaskDescriptor.ID_UNCHANGED leaves an id as it is.
///
/// An unprivileged task may only shuffle the three uids it already holds, and
/// either every id is written or none is.
pub fn do(ruid: u32, euid: u32, suid: u32) !void {
    const task = scheduler.get_current_task();

    if (!task.is_privileged()) {
        for ([_]u32{ ruid, euid, suid }) |id| {
            if (id == TaskDescriptor.ID_UNCHANGED) continue;
            if (id != task.uid and id != task.euid and id != task.suid)
                return error.EPERM;
        }
    }

    if (ruid != TaskDescriptor.ID_UNCHANGED) task.uid = ruid;
    if (euid != TaskDescriptor.ID_UNCHANGED) task.euid = euid;
    if (suid != TaskDescriptor.ID_UNCHANGED) task.suid = suid;
}
