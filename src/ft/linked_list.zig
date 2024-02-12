pub fn DoublyLinkedList(comptime T: type) type {
	return struct {
		const Self = @This();
		pub const Node = struct {
			value: T,
			next: ?*Node = null,
			prev: ?*Node = null
		};
		first: ?*Node = null,
		last: ?*Node = null,
		len: usize = 0,

		pub fn append(list: *Self, new_node: *Node) void {
			// TODO
			_ = list; _ = new_node;
		}

		pub fn concatByMoving(list1: *Self, list2: *Self) void {
			// TODO
			_ = list1; _ = list2;
		}

		pub fn insertAfter(list: *Self, node: *Node, new_node: *Node) void {
			// TODO
			_ = list; _ = node; _ = new_node;
		}

		pub fn insertBefore(list: *Self, node: *Node, new_node: *Node) void {
			// TODO
			_ = list; _ = node; _ = new_node;
		}

		pub fn pop(list: *Self) ?*Node {
			// TODO
			_ = list;
			return null;
		}

		pub fn popFirt(list: *Self) ?*Node {
			// TODO
			_ = list;
			return null;
		}

		pub fn prepend(list: *Self, new_node: *Node) void {
			// TODO
			_ = list; _ = new_node;
		}

		pub fn remove(list: *Self, node: *Node) void {
			// TODO
			_ = list; _ = node;
		}
	};
}