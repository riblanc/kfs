pub fn DoublyLinkedList(comptime T: type) type {
	return struct {
		const Self = @This();
		pub const Node = struct {
			data: T,
			next: ?*Node = null,
			prev: ?*Node = null
		};
		first: ?*Node = null,
		last: ?*Node = null,
		len: usize = 0,

		pub fn concatByMoving(list1: *Self, list2: *Self) void {
			// TODO
			_ = list1; _ = list2;
			@compileError("Not implemented");
		}

		pub fn insertAfter(list: *Self, node: *Node, new_node: *Node) void {
			new_node.prev = node;
			if (node.next) |n| {
				new_node.next = n;
				n.prev = new_node;
			} else {
				list.last = new_node;
			}
			node.next = new_node;
			list.len += 1;
		}

		pub fn insertBefore(list: *Self, node: *Node, new_node: *Node) void {
			new_node.next = node;
			if (node.prev) |n| {
				new_node.prev = n;
				n.next = new_node;
			} else {
				list.first = new_node;
			}
			node.prev = new_node;
			list.len += 1;
		}

		pub fn pop(list: *Self) ?*Node {
			if (list.last) |n| {
				list.remove(n);
				return n;
			} else {
				return null;
			}
		}

		pub fn popFirst(list: *Self) ?*Node {
			if (list.first) |n| {
				list.remove(n);
				return n;
			} else {
				return null;
			}
		}

		pub fn append(list: *Self, new_node: *Node) void {
			if (list.last) |n| {
				list.insertAfter(n, new_node);
			} else {
				list.first = new_node;
				list.last = new_node;
				list.len += 1;
			}
		}

		pub fn prepend(list: *Self, new_node: *Node) void {
			if (list.first) |n| {
				list.insertBefore(n, new_node);
			} else {
				list.first = new_node;
				list.last = new_node;
				list.len += 1;
			}
		}

		pub fn remove(list: *Self, node: *Node) void {
			if (node.prev) |n| {
				n.next = node.next;
			} else {
				list.first = node.next;
			}
			if (node.next) |n| {
				n.prev = node.prev;
			} else {
				list.last = node.prev;
			}
			list.len -= 1;
		}
	};
}

test "basic DoublyLinkedList test" {
	const testing = @import("std").testing;
    const L = DoublyLinkedList(u32);
    var list = L{};

    var one = L.Node{ .data = 1 };
    var two = L.Node{ .data = 2 };
    var three = L.Node{ .data = 3 };
    var four = L.Node{ .data = 4 };
    var five = L.Node{ .data = 5 };

    list.append(&two); // {2}

    list.append(&five); // {2, 5}

    list.prepend(&one); // {1, 2, 5}

    list.insertBefore(&five, &four); // {1, 2, 4, 5}

    list.insertAfter(&two, &three); // {1, 2, 3, 4, 5}


    // Traverse forwards.

    {
        var it = list.first;
        var index: u32 = 1;
        while (it) |node| : (it = node.next) {
            try testing.expect(node.data == index);
            index += 1;
        }
    }

    // Traverse backwards.

    {
        var it = list.last;
        var index: u32 = 1;
        while (it) |node| : (it = node.prev) {
            try testing.expect(node.data == (6 - index));
            index += 1;
        }
    }

    _ = list.popFirst(); // {2, 3, 4, 5}

    _ = list.pop(); // {2, 3, 4}

    list.remove(&three); // {2, 4}


    try testing.expect(list.first.?.data == 2);
    try testing.expect(list.last.?.data == 4);
    try testing.expect(list.len == 2);
}