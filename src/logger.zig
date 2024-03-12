const log = @import("ft/ft.zig").log;
const tty = @import("tty/tty.zig");
const utils = @import("shell/utils.zig");
const screen_of_death = @import("screen_of_death.zig").screen_of_death;

pub fn kernel_log(
	comptime message_level: log.Level,
	comptime scope: anytype,
	comptime format: []const u8,
	args: anytype
) void {
	if (message_level == .err and scope == .default)
		return screen_of_death(format, args);
	const level_str = comptime message_level.asText();
	const scope_str = if (scope != .default) (@tagName(scope) ++ ": ") else "";
	const color = switch (message_level) {
		.debug => utils.cyan,
		.info => utils.green,
		.warn => utils.yellow,
		.err => utils.red,
	};
	const padding = 7 - level_str.len;

	tty.printk(
		"[" ++ color ++ level_str ++ utils.reset ++ "] " ++
		(" " ** padding) ++ scope_str ++ format ++ "\n",
		args,
	);
}