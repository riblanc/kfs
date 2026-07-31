/* Lists directories, one entry per line. No columns and no sorting: the point
 * is to exercise opendir, readdir and stat, not to be ls. */

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

static int failed;
static int show_hidden;
static int long_format;

static char type_letter(unsigned char type) {
	switch (type) {
	case DT_DIR: return 'd';
	case DT_CHR: return 'c';
	case DT_BLK: return 'b';
	case DT_LNK: return 'l';
	case DT_FIFO: return 'p';
	case DT_SOCK: return 's';
	default: return '-';
	}
}

static void permissions(mode_t mode, char *out) {
	static const char bits[] = "rwx";

	for (int i = 0; i < 9; ++i)
		out[i] = (mode & (0400 >> i)) ? bits[i % 3] : '-';
	out[9] = '\0';
}

/* The directory entry carries a type, but nothing else, so anything more than
 * a name needs a second trip through the filesystem. */
static void print_long(const char *dir, struct dirent *entry) {
	char path[1024];
	struct stat info;
	char perms[10];

	if (strcmp(dir, ".") == 0)
		snprintf(path, sizeof path, "%s", entry->d_name);
	else
		snprintf(path, sizeof path, "%s/%s", dir, entry->d_name);

	if (stat(path, &info) < 0) {
		printf("?????????  %8s  %s\n", "?", entry->d_name);
		return;
	}
	permissions(info.st_mode, perms);
	printf("%c%s  %8lld  %s\n", type_letter(entry->d_type), perms,
		(long long)info.st_size, entry->d_name);
}

static void list(const char *path, int with_header) {
	DIR *dir = opendir(path);

	if (!dir) {
		fprintf(stderr, "ls: %s: %s\n", path, strerror(errno));
		failed = 1;
		return;
	}
	if (with_header)
		printf("%s:\n", path);

	for (;;) {
		errno = 0;
		struct dirent *entry = readdir(dir);
		if (!entry)
			break;
		if (!show_hidden && entry->d_name[0] == '.')
			continue;
		if (long_format)
			print_long(path, entry);
		else
			printf("%s\n", entry->d_name);
	}
	closedir(dir);
}

int main(int argc, char **argv) {
	const char *paths[64];
	int count = 0;

	for (int i = 1; i < argc; ++i) {
		if (argv[i][0] == '-' && argv[i][1]) {
			for (const char *o = argv[i] + 1; *o; ++o) {
				if (*o == 'a')
					show_hidden = 1;
				else if (*o == 'l')
					long_format = 1;
				else {
					fprintf(stderr, "ls: unknown option -%c\n", *o);
					return 2;
				}
			}
		} else if (count < (int)(sizeof paths / sizeof *paths)) {
			paths[count++] = argv[i];
		}
	}

	if (count == 0)
		list(".", 0);
	else
		for (int i = 0; i < count; ++i)
			list(paths[i], count > 1);
	return failed;
}
