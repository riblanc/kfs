/* Counts lines, words and bytes. Reads standard input when given no file. */

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static int failed;

static void count(int fd, const char *name, int with_name) {
	char buffer[4096];
	unsigned long lines = 0, words = 0, bytes = 0;
	int in_word = 0;

	for (;;) {
		ssize_t got = read(fd, buffer, sizeof buffer);
		if (got < 0) {
			if (errno == EINTR)
				continue;
			fprintf(stderr, "wc: %s: %s\n", name, strerror(errno));
			failed = 1;
			return;
		}
		if (got == 0)
			break;
		for (ssize_t i = 0; i < got; ++i) {
			++bytes;
			if (buffer[i] == '\n')
				++lines;
			if (isspace((unsigned char)buffer[i])) {
				in_word = 0;
			} else if (!in_word) {
				in_word = 1;
				++words;
			}
		}
	}
	printf("%7lu %7lu %7lu", lines, words, bytes);
	if (with_name)
		printf(" %s", name);
	putchar('\n');
}

int main(int argc, char **argv) {
	if (argc < 2) {
		count(0, "standard input", 0);
		return failed;
	}
	for (int i = 1; i < argc; ++i) {
		FILE *file = fopen(argv[i], "r");
		if (!file) {
			fprintf(stderr, "wc: %s: %s\n", argv[i], strerror(errno));
			failed = 1;
			continue;
		}
		count(fileno(file), argv[i], 1);
		fclose(file);
	}
	return failed;
}
