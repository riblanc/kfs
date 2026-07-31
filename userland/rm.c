/* Removes names. Directories are refused: there is no rmdir yet. */

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv) {
	int failed = 0;

	if (argc < 2) {
		fprintf(stderr, "rm: missing operand\n");
		return 1;
	}
	for (int i = 1; i < argc; ++i) {
		if (unlink(argv[i]) < 0) {
			fprintf(stderr, "rm: %s: %s\n", argv[i], strerror(errno));
			failed = 1;
		}
	}
	return failed;
}
