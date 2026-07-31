const utils = @import("../utils.zig");
pub const Shell = @import("../Shell.zig").Shell(@import("builtins.zig"));
const colors = @import("colors");
const tty = @import("../../device/tty/tty.zig");

pub var cwd: []const u8 = undefined;

/// One cwd for every shell, so only the first one to start allocates it.
var cwd_ready = false;

pub fn on_init(shell: *Shell) void {
    shell.writer().print("tty {d}, Hello {s}{d}{s}\n", .{
        tty.index_of(shell.tty()),
        colors.green,
        42,
        colors.reset,
    }) catch {};
    shell.tty().config.c_lflag.ECHOCTL = true;
    if (!cwd_ready) {
        cwd = @import("../../memory.zig").smallAlloc.allocator().dupe(u8, "/") catch unreachable;
        cwd_ready = true;
    }
}

pub fn on_error(shell: *Shell) void {
    utils.ensure_newline(shell.writer());
    shell.defaultErrorHook();
}

pub fn pre_process(shell: *Shell) void {
    tty.flush();
    utils.print_prompt(shell);
}

pub fn pre_cmd(shell: *Shell) void {
    utils.ensure_newline(shell.writer());
}
