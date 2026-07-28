const scheduler = @import("../task/scheduler.zig");
const signal = @import("../task/signal.zig");

pub const Id = 8;

pub fn do(id: signal.Id, handler: signal.Handler) !void {
    try scheduler.get_current_task().signalManager.change_action(
        id,
        .{ .handler = .{ .sa_handler = handler } },
    );
}
