const scheduler = @import("../task/scheduler.zig");
const signal = @import("../task/signal.zig");

pub const Id = 8;

/// Set what a signal does, ANSI signal.
pub fn do(signo: u32, handler: signal.Handler) !void {
    const id = signal.Id.from(signo) orelse return error.EINVAL;
    try scheduler.get_current_task().signalManager.change_action(id, .{ .sa_handler = handler });
}
