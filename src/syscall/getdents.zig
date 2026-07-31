const std = @import("std");
const scheduler = @import("../task/scheduler.zig");
const TaskDescriptor = @import("../task/task.zig").TaskDescriptor;
const File = @import("../fs/file.zig");
const abi = @import("../fs/abi.zig");
const Errno = @import("../errno.zig").Errno;

pub const Id = 35;

/// Fills the buffer with as many records as fit, each stepped over with its own
/// d_reclen. Records are padded to the alignment of the struct, since userspace
/// reads them straight out of the buffer.
pub fn do(fd: TaskDescriptor.Fd, buffer: [*]u8, size: usize) !usize {
    const file = scheduler.get_current_task().get_file(fd) orelse return Errno.EBADF;
    var written: usize = 0;

    while (true) {
        // A record that does not fit has to stay unread, and the only way back
        // is the position the directory was at before it was consumed.
        const previous = file.pos;
        var entry: File.DirEnt = undefined;
        if (!try file.readdir(&entry))
            break;

        const name = entry.name[0..entry.name_len];
        const length = std.mem.alignForward(usize, abi.Dirent.size_for(name.len), @alignOf(abi.Dirent));
        if (written + length > size) {
            file.pos = previous;
            // Not even one record fits, and the caller would loop forever on an
            // empty answer.
            if (written == 0)
                return Errno.EINVAL;
            break;
        }

        const record: *abi.Dirent = @ptrCast(@alignCast(buffer + written));
        record.d_ino = entry.inode;
        record.d_off = @intCast(file.pos);
        record.d_reclen = @intCast(length);
        // Mode.Type already holds the nibble the DT_ names stand for.
        record.d_type = @intFromEnum(entry.type);
        @memcpy(record.d_name[0..name.len], name);
        record.d_name[name.len] = 0;
        written += length;
    }
    return written;
}
