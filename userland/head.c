/* Shows the first lines of each file, ten by default. */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int failed;

static void show(FILE *file, const char *name, long limit) {
	int c;
	long lines = 0;

	while (lines < limit && (c = fgetc(file)) != EOF) {
		putchar(c);
		if (c == '\n')
			++lines;
	}
	(void)name;
}

int main(int argc, char **argv) {
	long limit = 10;
	int first = 1;

	if (argc > 2 && strcmp(argv[1], "-n") == 0) {
		limit = strtol(argv[2], NULL, 10);
		first = 3;
	}
	if (first >= argc) {
		show(stdin, "standard input", limit);
		return failed;
	}
	for (int i = first; i < argc; ++i) {
		FILE *file = fopen(argv[i], "r");
		if (!file) {
			fprintf(stderr, "head: %s: %s\n", argv[i], strerror(errno));
			failed = 1;
			continue;
		}
		if (argc - first > 1)
			printf("==> %s <==\n", argv[i]);
		show(file, argv[i], limit);
		fclose(file);
	}
	return failed;
}
