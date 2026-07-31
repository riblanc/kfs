const DefaultShell = @import("shell/default/shell.zig");
const tty = @import("./device/tty/tty.zig");
const TtyStruct = @import("./device/tty/tty_struct.zig");

fn run_shell(t: *TtyStruct) noreturn {
    var shell = DefaultShell.Shell.init(t, .{}, .{
        .on_init = DefaultShell.on_init,
        .pre_prompt = DefaultShell.pre_process,
    });
    while (true) shell.process_line();
}

fn shell_entry(index: usize) u8 {
    run_shell(&tty.tty_array[index]);
}

pub fn main(_: usize) u8 {
    @import("std").log.debug("bonjour", .{});
    @import("fs/vfs.zig").scan();

    // A shell on every terminal that has a driver behind it. Keystrokes only
    // reach the displayed console, so the others sit blocked on their own read
    // until Alt+F brings one up. What a shell runs inherits its terminal, so
    // two programs can print at once without sharing a scrollback.
    const task_set = @import("task/task_set.zig");
    for (1..tty.total_ttys) |i| {
        if (!tty.tty_array[i].attached())
            continue;
        const task = task_set.create_task() catch @panic("Failed to create shell task");
        task.spawn(&shell_entry, i) catch @panic("Failed to spawn shell task");
    }

    run_shell(&tty.tty_array[0]);
}
