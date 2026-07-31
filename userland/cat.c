/* Copies files to standard output, or standard input when given none. */

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static int failed;

static void copy(int fd, const char *name) {
	char buffer[4096];

	for (;;) {
		ssize_t got = read(fd, buffer, sizeof buffer);
		if (got == 0)
			return;
		if (got < 0) {
			if (errno == EINTR)
				continue;
			fprintf(stderr, "cat: %s: %s\n", name, strerror(errno));
			failed = 1;
			return;
		}
		/* A short write is not a failure, so keep pushing what is left. */
		ssize_t done = 0;
		while (done < got) {
			ssize_t put = write(1, buffer + done, (size_t)(got - done));
			if (put < 0) {
				if (errno == EINTR)
					continue;
				fprintf(stderr, "cat: write: %s\n", strerror(errno));
				failed = 1;
				return;
			}
			done += put;
		}
	}
}

int main(int argc, char **argv) {
	if (argc < 2) {
		copy(0, "standard input");
		return failed;
	}
	for (int i = 1; i < argc; ++i) {
		if (strcmp(argv[i], "-") == 0) {
			copy(0, "standard input");
			continue;
		}
		FILE *file = fopen(argv[i], "r");
		if (!file) {
			fprintf(stderr, "cat: %s: %s\n", argv[i], strerror(errno));
			failed = 1;
			continue;
		}
		copy(fileno(file), argv[i]);
		fclose(file);
	}
	return failed;
}
