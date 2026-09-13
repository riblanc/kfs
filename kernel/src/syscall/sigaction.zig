const scheduler = @import("../task/scheduler.zig");
const signal = @import("../task/signal.zig");

pub const Id = 11;

/// Read and set what a signal does, POSIX sigaction. A number that names no
/// signal, or one that cannot be caught, is refused rather than acted on.
pub fn do(signo: u32, act: ?*signal.Sigaction, oldact: ?*signal.Sigaction) !void {
    const id = signal.Id.from(signo) orelse return error.EINVAL;
    const manager = &scheduler.get_current_task().signalManager;

    if (oldact) |out| out.* = manager.get_action(id);
    if (act) |new| try manager.change_action(id, new.*);
}
