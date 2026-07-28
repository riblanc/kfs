const scheduler = @import("../task/scheduler.zig");
const signal = @import("../task/signal.zig");

pub const Id = 11;

pub fn do(id: signal.Id, act: ?*signal.Sigaction, oldact: ?*signal.Sigaction) !void {
    const manager = &scheduler.get_current_task().signalManager;
    // SIGKILL and SIGSTOP refuse to be handled, which is an answer to give back
    // rather than a reason to stop the kernel. Read the old action before the
    // change but write it after, so a refused call leaves both untouched.
    const previous = manager.get_action(id);
    if (act) |act_ptr| {
        try manager.change_action(id, act_ptr.*);
    }
    if (oldact) |oldact_ptr| {
        oldact_ptr.* = previous;
    }
}
