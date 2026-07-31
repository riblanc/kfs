/* Copies one file onto another. No directories, no recursion. */

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv) {
	char buffer[4096];

	if (argc != 3) {
		fprintf(stderr, "cp: usage: cp source destination\n");
		return 1;
	}
	int in = open(argv[1], O_RDONLY);
	if (in < 0) {
		fprintf(stderr, "cp: %s: %s\n", argv[1], strerror(errno));
		return 1;
	}
	int out = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 00644);
	if (out < 0) {
		fprintf(stderr, "cp: %s: %s\n", argv[2], strerror(errno));
		close(in);
		return 1;
	}
	for (;;) {
		ssize_t got = read(in, buffer, sizeof buffer);
		if (got == 0)
			break;
		if (got < 0) {
			if (errno == EINTR)
				continue;
			fprintf(stderr, "cp: read: %s\n", strerror(errno));
			close(in), close(out);
			return 1;
		}
		ssize_t done = 0;
		while (done < got) {
			ssize_t put = write(out, buffer + done, (size_t)(got - done));
			if (put < 0) {
				if (errno == EINTR)
					continue;
				fprintf(stderr, "cp: write: %s\n", strerror(errno));
				close(in), close(out);
				return 1;
			}
			done += put;
		}
	}
	close(in);
	close(out);
	return 0;
}
