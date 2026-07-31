/* Prints the working directory. */

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(void) {
	char path[PATH_MAX];

	if (!getcwd(path, sizeof path)) {
		fprintf(stderr, "pwd: %s\n", strerror(errno));
		return 1;
	}
	printf("%s\n", path);
	return 0;
}
