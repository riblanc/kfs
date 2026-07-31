// The character device registry, presented as a directory.
//
// Nothing here is stored on any medium. The root lists whatever the registry
// holds at the moment it is asked, and an inode is a name for a dev_t, so a
// terminal appearing or going away needs no bookkeeping on this side.
const std = @import("std");

const Errno = @import("../errno.zig").Errno;
const FileSystem = @import("filesystem.zig");
const Inode = @import("inode.zig");
const Partition = @import("../device/block/partition.zig");
const SuperBlock = @import("superblock.zig");
const CharDevice = @import("../device/char/cdev.zig");
const registry = @import("../device/char/registry.zig");
const types = @import("../device/types.zig");

const allocator = @import("../memory.zig").smallAlloc.allocator();
const logger = std.log.scoped(.devfs);

/// The root takes ino 1 and a device takes its dev_t moved past it, so an inode
/// number says which device it stands for and no table has to be kept in step.
const root_ino: Inode.Ino = 1;

fn ino_of(devt: types.dev_t) Inode.Ino {
    return @as(Inode.Ino, devt.toInt()) + root_ino + 1;
}

fn devt_of(ino: Inode.Ino) types.dev_t {
    return types.dev_t.fromInt(@intCast(ino - root_ino - 1));
}

// Device inodes

fn device_of(inode: *Inode) ?*CharDevice {
    return registry.get_device(inode.type_specific.Character);
}

/// The position File keeps is ignored: a terminal has no offset to read from.
fn device_pread(self: *Inode, _: u64, buffer: []u8) Inode.Error.pread!usize {
    const dev = device_of(self) orelse return Errno.ENODEV;
    const read_fn = dev.ops.read orelse return Errno.EPERM;
    return read_fn(dev, buffer) catch |e| {
        // A signal cutting a read short has to reach the caller as such: zero
        // is how the end of a stream is reported, and a shell reading a line
        // would take it for one.
        if (e == error.Interrupted)
            return error.EINTR;
        logger.debug("{s}: read failed, {s}", .{ std.mem.sliceTo(&dev.name, 0), @errorName(e) });
        return Errno.EIO;
    };
}

fn device_pwrite(self: *Inode, _: u64, buffer: []const u8) Inode.Error.pwrite!usize {
    const dev = device_of(self) orelse return Errno.ENODEV;
    const write_fn = dev.ops.write orelse return Errno.EPERM;
    return write_fn(dev, buffer) catch |e| {
        logger.debug("{s}: write failed, {s}", .{ std.mem.sliceTo(&dev.name, 0), @errorName(e) });
        return Errno.EIO;
    };
}

fn not_a_directory(_: *Inode, _: u64, _: *Inode.DirEnt) Inode.Error.preaddir!usize {
    return Errno.EPERM;
}

// Root directory

fn root_lookup(self: *Inode, name: []const u8) Inode.Error.lookup!?*Inode {
    const dev = registry.get_device_by_name(name) orelse return null;
    return try self.superblock.retrieve_inode(ino_of(dev.devt));
}

fn root_preaddir(self: *Inode, pos: u64, dst: *Inode.DirEnt) Inode.Error.preaddir!usize {
    // Reread the count every time: a device may have been registered since the
    // directory was opened, and readdir stops on this size.
    self.size = registry.count();

    var it = registry.iterator();
    var index: u64 = 0;
    while (it.next()) |entry| : (index += 1) {
        if (index != pos)
            continue;
        const dev = entry.key;
        const name = std.mem.sliceTo(&dev.name, 0);
        dst.inode = ino_of(dev.devt);
        dst.type = .Character;
        dst.name_len = name.len;
        @memcpy(dst.name[0..name.len], name);
        return 1;
    }
    return 0;
}

// Nothing here is created, renamed or resized: the registry decides what exists.

fn flush(_: *Inode) Inode.Error.flush!void {}

fn truncate(_: *Inode, _: u64) Inode.Error.truncate!void {
    return Errno.EPERM;
}

fn link(_: *Inode, _: []const u8, _: *Inode) Inode.Error.link!void {
    return Errno.EPERM;
}

fn unlink(_: *Inode, _: []const u8) Inode.Error.unlink!void {
    return Errno.EPERM;
}

fn no_pread(_: *Inode, _: u64, _: []u8) Inode.Error.pread!usize {
    return Errno.EPERM;
}

fn no_pwrite(_: *Inode, _: u64, _: []const u8) Inode.Error.pwrite!usize {
    return Errno.EPERM;
}

const device_vtable: Inode.VTable = .{
    .flush = &flush,
    .truncate = &truncate,
    .link = &link,
    .unlink = &unlink,
    .pread = &device_pread,
    .pwrite = &device_pwrite,
    .preaddir = &not_a_directory,
};

const root_vtable: Inode.VTable = .{
    .flush = &flush,
    .lookup = &root_lookup,
    .truncate = &truncate,
    .link = &link,
    .unlink = &unlink,
    .pread = &no_pread,
    .pwrite = &no_pwrite,
    .preaddir = &root_preaddir,
};

// Superblock

const rw = Inode.Mode.Perm{ .read = true, .write = true };
const rx = Inode.Mode.Perm{ .read = true, .execute = true };

fn load_inode(superblock: *SuperBlock, ino: Inode.Ino) SuperBlock.Error.load_inode!*Inode {
    const inode = allocator.create(Inode) catch return error.ENOMEM;
    errdefer allocator.destroy(inode);

    if (ino == root_ino) {
        inode.* = .{
            .superblock = superblock,
            .ino = ino,
            .hard_links = 2,
            .size = registry.count(),
            .uid = 0,
            .gid = 0,
            .mode = .{ .type = .Directory, .owner = rx, .group = rx, .other = rx },
            .type_specific = .{ .Directory = .{} },
            .vtable = &root_vtable,
        };
        return inode;
    }

    const devt = devt_of(ino);
    if (registry.get_device(devt) == null)
        return error.ENODEV;

    inode.* = .{
        .superblock = superblock,
        .ino = ino,
        .hard_links = 1,
        .size = 0,
        .uid = 0,
        .gid = 0,
        .mode = .{ .type = .Character, .owner = rw, .group = rw, .other = rw },
        .type_specific = .{ .Character = devt },
        .vtable = &device_vtable,
    };
    return inode;
}

fn release_inode(_: *SuperBlock, inode: *Inode) SuperBlock.Error.release_inode!void {
    allocator.destroy(inode);
}

fn get_root(superblock: *SuperBlock) SuperBlock.Error.get_root!*Inode {
    return superblock.retrieve_inode(root_ino);
}

fn create_inode(
    _: *SuperBlock,
    _: Inode.Uid,
    _: Inode.Gid,
    _: Inode.Mode,
    _: SuperBlock.TypeSpecificParams,
) SuperBlock.Error.create_inode!*Inode {
    // mknod has nothing to write to: a node exists here only while its driver
    // is registered.
    return error.EPERM;
}

const superblock_vtable: SuperBlock.VTable = .{
    .load_inode = &load_inode,
    .release_inode = &release_inode,
    .get_root = &get_root,
    .create_inode = &create_inode,
};

pub fn create(_: ?*Partition, alloc: std.mem.Allocator) *SuperBlock {
    const superblock = alloc.create(SuperBlock) catch @panic("devfs: out of memory");
    superblock.* = .{
        .block_size = 1,
        .fragment_size = 1,
        .blocks = 0,
        .free_blocks = 0,
        .reserved_blocks = 0,
        .files = registry.count(),
        .free_files = 0,
        .reserved_files = 0,
        .fsid = null,
        .flags = .{ .read_only = false, .no_suid = true },
        .max_name = CharDevice.CDEV_NAME_LEN - 1,
        .uuid = null,
        .partition = null,
        .vtable = &superblock_vtable,
        .cache = SuperBlock.InodeCache.init(alloc),
    };
    return superblock;
}

pub const fs: FileSystem = .{
    .create = &create,
    .name = "devfs",
};
