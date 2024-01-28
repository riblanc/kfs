const tty = @import("../tty/tty.zig");
const u = @import("utils.zig");

const Help = struct {
	name:  [:0]const u8,
	description: [:0]const u8,
	usage: ?[:0]const u8 = null
};

fn print_helper(h: Help) void {
	tty.printk(u.blue ++ "Command" ++ u.reset ++ ": {s}\n",  .{
		h.name
	});
	tty.printk(u.blue ++ "Description" ++ u.reset ++ ": {s}\n",  .{
		h.description
	});
	if (h.usage) |usage| {
		tty.printk(u.blue ++ "Usage" ++ u.reset ++ ": {s}\n",  .{usage});
	}
}

pub fn stack() void {
	print_helper(Help{
		.name = "stack",
		.description =
			"Prints the stack.\n" ++
			u.yellow ++ "WARNING" ++ u.reset ++ ": This command is not implemented yet.",
		.usage = null
	});
}

pub fn help() void {
	print_helper(Help{
		.name = "help",
		.description = "Prints the help message",
		.usage = "help <command>"
	});
}