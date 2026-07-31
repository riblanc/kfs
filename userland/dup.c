/* Exercises dup and dup2. What matters is that both descriptors name one open
 * file, so writes through either land on the same device and closing one leaves
 * the other alive. */

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static int failures;

static void check(const char *what, int ok) {
	printf("%-44s %s\n", what, ok ? "ok" : "FAILED");
	if (!ok)
		failures++;
}

int main(void) {
	setbuf(stdout, NULL);

	int fd = open("/dev/tty1", O_RDWR);
	check("open(/dev/tty1)", fd >= 0);
	if (fd < 0)
		return 1;

	int copy = dup(fd);
	check("dup returns a new descriptor", copy >= 0 && copy != fd);

	const char msg[] = "through the dup\n";
	check("write through the copy", write(copy, msg, sizeof msg - 1) == (ssize_t)(sizeof msg - 1));

	/* Closing one handle must leave the other usable: the open file goes away
	 * only with the last of them. */
	check("close the original", close(fd) == 0);
	check("the copy still writes", write(copy, msg, sizeof msg - 1) == (ssize_t)(sizeof msg - 1));

	/* While the number is still free: the next open takes it back, so this has
	 * to be asked before anything else is opened. */
	errno = 0;
	check("dup of a closed descriptor fails", dup(fd) < 0);

	/* dup2 onto a descriptor in use closes it first and returns the number
	 * asked for, not one of its choosing. */
	int target = open("/dev/tty2", O_RDWR);
	check("open(/dev/tty2)", target >= 0);
	check("open reuses the freed number", target == fd);
	check("dup2 returns the requested number", dup2(copy, target) == target);
	check("the redirected descriptor writes", write(target, msg, sizeof msg - 1) == (ssize_t)(sizeof msg - 1));

	check("dup2 onto itself is a no-op", dup2(target, target) == target);
	check("and left it open", write(target, msg, sizeof msg - 1) > 0);

	check("close the copy", close(copy) == 0);
	check("close the target", close(target) == 0);

	printf("\n%s\n", failures ? "some checks failed" : "all checks passed");
	return failures != 0;
}
