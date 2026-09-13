const scheduler = @import("../task/scheduler.zig");
const signal = @import("../task/signal.zig");
const TaskDescriptor = @import("../task/task.zig").TaskDescriptor;
const wait_queue = @import("../task/wait_queue.zig");

pub const Id = 67;

/// A signal the caller does not block is what ends the wait.
fn deliverable(task: *void, _: ?*void) bool {
    const descriptor: *TaskDescriptor = @ptrCast(@alignCast(task));
    return descriptor.signalManager.peek_pending(descriptor.ucontext.uc_sigmask) != null;
}

var queue: wait_queue.WaitQueue(.{ .predicate = deliverable }) = .{};

/// Wait for a signal under another mask, POSIX sigsuspend. Always fails with
/// EINTR: the call has no other way to return.
///
/// The mask is put back before the handler runs rather than after, the kernel
/// having no place to keep it across sigreturn.
pub fn do(set: *const signal.SigSet) !void {
    const task = scheduler.get_current_task();
    const saved = task.ucontext.uc_sigmask;

    task.ucontext.uc_sigmask = set.*;
    queue.block(task, null) catch {};
    task.ucontext.uc_sigmask = saved;

    return error.EINTR;
}
