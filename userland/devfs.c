/* Exercises the Open and Close sysdeps against devfs: a device node opened by
 * name, written through, and the errors that have to come back. */

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static int failures;

static void check(const char *what, int ok) {
	printf("%-42s %s\n", what, ok ? "ok" : "FAILED");
	if (!ok)
		failures++;
}

int main(int argc, char **argv) {
	/* Whichever terminal is not the one this runs on, so the writes below are
	 * visible somewhere else. */
	const char *path = (argc > 1) ? argv[1] : "/dev/tty1";

	setbuf(stdout, NULL);

	int fd = open(path, O_RDWR);
	check("open(/dev/ttyN, O_RDWR)", fd >= 0);
	if (fd < 0) {
		printf("errno %d\n", errno);
		return 1;
	}
	check("the fd is past the standard three", fd >= 3);

	const char msg[] = "written through devfs\n";
	ssize_t n = write(fd, msg, sizeof msg - 1);
	check("write reaches the device", n == (ssize_t)(sizeof msg - 1));

	check("close", close(fd) == 0);

	/* A name the registry does not hold. */
	errno = 0;
	check("open of a missing device fails", open("/dev/nope", O_RDONLY) < 0);

	/* The directory itself is not a device, and nothing may be created in it. */
	errno = 0;
	check("creating in /dev is refused", open("/dev/new", O_RDWR | O_CREAT, 0666) < 0);

	printf("\n%s\n", failures ? "some checks failed" : "all checks passed");
	return failures != 0;
}
