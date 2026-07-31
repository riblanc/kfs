const scheduler = @import("../task/scheduler.zig");
const wait_queue = @import("../task/wait_queue.zig");
const TaskDescriptor = @import("../task/task.zig").TaskDescriptor;
const Errno = @import("../errno.zig").Errno;

pub const Id = 26;

fn signal_pending(task: *void, _: ?*void) bool {
    const descriptor: *TaskDescriptor = @ptrCast(@alignCast(task));
    return descriptor.signalManager.has_pending();
}

/// One queue for every paused task: send_signal takes a task out of whichever
/// queue holds it, so they do not need one each.
var queue: wait_queue.WaitQueue(.{ .predicate = signal_pending }) = .{};

/// Waits for a signal and has no other outcome, so it reports EINTR whether one
/// was already pending or the task had to sleep for it. The handler itself runs
/// on the way back out, before userspace sees this.
pub fn do() !void {
    queue.block(scheduler.get_current_task(), null) catch {};
    return Errno.EINTR;
}
