// The shapes userspace sees, laid out as abi-bits describes them so they cross
// the syscall boundary unchanged.
//
// Every 64 bit field is aligned on 4 here, not 8: that is what the System V
// i386 ABI asks for, and it is why the offsets below are not what they would be
// anywhere else. The asserts pin them, since nothing else would catch a drift.
const std = @import("std");

const Inode = @import("inode.zig");

pub const Timespec = extern struct {
    tv_sec: i64 align(4) = 0,
    tv_nsec: i32 = 0,
    _reserved: i32 = 0,
};

pub const Stat = extern struct {
    st_dev: u64 align(4) = 0,
    st_ino: u64 align(4) = 0,
    st_mode: u32 = 0,
    st_nlink: u32 = 0,
    st_uid: u32 = 0,
    st_gid: u32 = 0,
    st_rdev: u64 align(4) = 0,
    st_size: i64 align(4) = 0,
    st_blksize: i32 = 0,
    st_blocks: i64 align(4) = 0,
    st_atim: Timespec = .{},
    st_mtim: Timespec = .{},
    st_ctim: Timespec = .{},

    comptime {
        std.debug.assert(@offsetOf(Stat, "st_ino") == 8);
        std.debug.assert(@offsetOf(Stat, "st_mode") == 16);
        std.debug.assert(@offsetOf(Stat, "st_rdev") == 32);
        std.debug.assert(@offsetOf(Stat, "st_size") == 40);
        std.debug.assert(@offsetOf(Stat, "st_blocks") == 52);
        std.debug.assert(@offsetOf(Stat, "st_atim") == 60);
        std.debug.assert(@offsetOf(Stat, "st_ctim") == 92);
        std.debug.assert(@sizeOf(Stat) == 108);
    }

    /// The file type lives in the high bits of st_mode, where the S_IF macros
    /// look for it, and the permission triples in the low ones.
    pub fn from_inode(inode: *const Inode) Stat {
        return .{
            .st_ino = inode.ino,
            .st_mode = (@as(u32, @intFromEnum(inode.mode.type)) << 12) |
                (@as(u32, @as(u9, @truncate(@as(u16, @bitCast(inode.mode))))) & 0o777),
            .st_nlink = inode.hard_links,
            .st_uid = inode.uid,
            .st_gid = inode.gid,
            .st_rdev = switch (inode.mode.type) {
                .Block => inode.type_specific.Block.toInt(),
                .Character => inode.type_specific.Character.toInt(),
                else => 0,
            },
            .st_size = @intCast(inode.size),
            .st_blksize = @intCast(inode.superblock.block_size),
        };
    }
};

/// Records are packed one after another and stepped over with d_reclen, so the
/// name is only as long as it needs to be.
pub const Dirent = extern struct {
    d_ino: u64 align(4),
    d_off: i64 align(4),
    d_reclen: u16,
    d_type: u8,
    d_name: [name_max + 1]u8,

    pub const name_max = 255;

    /// Bytes a record with this name takes, name and its terminator included.
    pub fn size_for(name_len: usize) usize {
        return @offsetOf(Dirent, "d_name") + name_len + 1;
    }

    comptime {
        std.debug.assert(@offsetOf(Dirent, "d_off") == 8);
        std.debug.assert(@offsetOf(Dirent, "d_reclen") == 16);
        std.debug.assert(@offsetOf(Dirent, "d_type") == 18);
        std.debug.assert(@offsetOf(Dirent, "d_name") == 19);
        std.debug.assert(@sizeOf(Dirent) == 276);
    }
};
