pub const Id = 58;
const scheduler = @import("../task/scheduler.zig");

pub fn do(uid : u32) !void {
    scheduler.get_current_task().uid = uid;
}
