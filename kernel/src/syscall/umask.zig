const scheduler = @import("../task/scheduler.zig");
const open = @import("open.zig");

pub const Id = 69;

/// Set the permission bits newly created files are denied, POSIX umask, and
/// report the ones denied so far.
pub fn do(mask: open.Mode) !u32 {
    const task = scheduler.get_current_task();
    const previous = task.umask;
    task.umask = @as(u32, @bitCast(mask)) & 0o777;
    return previous;
}
