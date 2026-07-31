MLIBC_SRC = $(CURDIR)/mlibc
MLIBC_BUILD = $(CURDIR)/mlibc-build
SYSROOT = $(CURDIR)/sysroot
MLIBC_CROSS_FILE = $(CURDIR)/toolchain/shadokos-i686.cross
# mlibc needs meson >= 1.3, which distro packages often predate. Install a
# recent one (pip install --user meson) and point MESON at it if needed.
MESON ?= meson
NINJA ?= ninja
# Only the zig cc wrappers are ours to provide.
MLIBC_PATH = $(CURDIR)/toolchain/bin:$(PATH)

$(MLIBC_BUILD)/headers/build.ninja:
	PATH="$(MLIBC_PATH)" $(MESON) setup \
		--cross-file $(MLIBC_CROSS_FILE) \
		--prefix=/usr \
		-Dheaders_only=true \
		$(MLIBC_BUILD)/headers $(MLIBC_SRC)

.PHONY: libc-headers
libc-headers: $(MLIBC_BUILD)/headers/build.ninja
	DESTDIR=$(SYSROOT) $(NINJA) -C $(MLIBC_BUILD)/headers install

$(MLIBC_BUILD)/libc/build.ninja:
	PATH="$(MLIBC_PATH)" $(MESON) setup \
		--cross-file $(MLIBC_CROSS_FILE) \
		--prefix=/usr \
		-Ddefault_library=static \
		-Dno_headers=true \
		-Dlibgcc_dependency=false \
		$(MLIBC_BUILD)/libc $(MLIBC_SRC)

.PHONY: libc
libc: libc-headers $(MLIBC_BUILD)/libc/build.ninja
	PATH="$(MLIBC_PATH)" $(NINJA) -C $(MLIBC_BUILD)/libc
	DESTDIR=$(SYSROOT) $(NINJA) -C $(MLIBC_BUILD)/libc install

USERLAND = $(CURDIR)/userland
USERLAND_OBJ = $(USERLAND)/.objs
USERLAND_BIN = $(USERLAND)/build
USERLAND_SRC = $(wildcard $(USERLAND)/*.c)
USERLAND_ELF = $(patsubst $(USERLAND)/%.c,$(USERLAND_BIN)/%,$(USERLAND_SRC))

# Programs are linked against libc.a, so a new sysdep has to relink them. The
# libc target itself stays order-only: it is phony and would rebuild everything
# on every run.
LIBC_ARCHIVE = $(SYSROOT)/usr/lib/libc.a

# The libc headers have to come before the compiler's own. zig cc targets
# freestanding, so __STDC_HOSTED__ is 0 and clang's limits.h and stdint.h stop
# short of the include_next that would reach mlibc's.
LIBC_INCLUDE = -I $(SYSROOT)/usr/include

$(USERLAND_OBJ)/%.o: $(USERLAND)/%.c $(LIBC_ARCHIVE) | libc
	mkdir -p $(USERLAND_OBJ)
	PATH="$(MLIBC_PATH)" i686-shadokos-cc \
		$(LIBC_INCLUDE) \
		-c $< -o $@

# zig cc intercepts -lc to provide its own libc, which does not exist for
# x86-freestanding, so libc.a is passed by path instead.
$(USERLAND_BIN)/%: $(USERLAND_OBJ)/%.o $(LIBC_ARCHIVE)
	mkdir -p $(USERLAND_BIN)
	PATH="$(MLIBC_PATH)" i686-shadokos-cc -nostdlib \
		$(SYSROOT)/usr/lib/crt1.o \
		$< \
		$(SYSROOT)/usr/lib/libc.a \
		-o $@

# Chained pattern rules make the objects intermediate, and make would delete
# them after linking.
.PRECIOUS: $(USERLAND_OBJ)/%.o

# A whole program rather than one file, so it gets its own rule. The readline
# directory holds a stand-in for the library shadokos does not have, and comes
# first on the include path so the shell's sources compile untouched.
MINISHELL_DIR = $(USERLAND)/minishell
MINISHELL_SRC = $(shell find $(MINISHELL_DIR) -name '*.c' 2>/dev/null)
MINISHELL_ELF = $(USERLAND_BIN)/minishell

# Installed alongside the one file programs.
USERLAND_ELF += $(MINISHELL_ELF)

$(MINISHELL_ELF): $(MINISHELL_SRC) $(LIBC_ARCHIVE) | libc
	mkdir -p $(USERLAND_BIN)
	PATH="$(MLIBC_PATH)" i686-shadokos-cc -nostdlib \
		-I $(MINISHELL_DIR) -I $(MINISHELL_DIR)/includes \
		$(LIBC_INCLUDE) \
		$(SYSROOT)/usr/lib/crt1.o \
		$(MINISHELL_SRC) \
		$(SYSROOT)/usr/lib/libc.a \
		-o $@

.PHONY: userland
userland: $(USERLAND_ELF)

# Nothing prunes what a build no longer produces, so a renamed program leaves
# its old binary behind, here and in the disk image.
.PHONY: userland-clean
userland-clean:
	rm -rf $(USERLAND_OBJ) $(USERLAND_BIN)

fclean: userland-clean

# Test disk mounted as root by the kernel, appended as a partition to kfs.iso.
# The UUID has to match the one limine.conf passes on the cmdline.
FS_IMAGE = $(CURDIR)/fs.iso
FS_UUID = 8581277f-6f63-447a-8d0c-347e180d2466
FS_SIZE = 32M

$(FS_IMAGE):
	truncate -s $(FS_SIZE) $@
	mke2fs -q -t ext2 -U $(FS_UUID) $@

# devfs is mounted here at boot. It only has to exist; whatever is inside is
# hidden by the mount.
.PHONY: fs-layout
fs-layout: $(FS_IMAGE)
	@debugfs -w -R "mkdir /dev" $(FS_IMAGE) >/dev/null 2>&1 || true
	@debugfs -w -R "mkdir /bin" $(FS_IMAGE) >/dev/null 2>&1 || true

# Every binary lands in /bin on the image. A stripped copy is written: the
# debug info would multiply the number of blocks exec() has to read.
.PHONY: userland-install
userland-install: userland $(FS_IMAGE) fs-layout
	@for elf in $(USERLAND_ELF); do \
		name=$$(basename $$elf); \
		strip -o $(USERLAND_BIN)/.$$name.stripped $$elf; \
		debugfs -w -R "rm /bin/$$name" $(FS_IMAGE) >/dev/null 2>&1 || true; \
		debugfs -w -R "write $(USERLAND_BIN)/.$$name.stripped /bin/$$name" $(FS_IMAGE); \
		rm -f $(USERLAND_BIN)/.$$name.stripped; \
	done
