/* Creates directories, one level at a time. */

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

int main(int argc, char **argv) {
	int failed = 0;

	if (argc < 2) {
		fprintf(stderr, "mkdir: missing operand\n");
		return 1;
	}
	for (int i = 1; i < argc; ++i) {
		if (mkdir(argv[i], 00755) < 0) {
			fprintf(stderr, "mkdir: %s: %s\n", argv[i], strerror(errno));
			failed = 1;
		}
	}
	return failed;
}
