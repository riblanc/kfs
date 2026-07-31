/* Creates the named files if they do not exist. There are no timestamps to
 * bring forward yet, so an existing file is left alone. */

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv) {
	int failed = 0;

	if (argc < 2) {
		fprintf(stderr, "touch: missing operand\n");
		return 1;
	}
	for (int i = 1; i < argc; ++i) {
		int fd = open(argv[i], O_WRONLY | O_CREAT, 00644);
		if (fd < 0) {
			fprintf(stderr, "touch: %s: %s\n", argv[i], strerror(errno));
			failed = 1;
			continue;
		}
		close(fd);
	}
	return failed;
}
