/* Does by hand what a shell does for `2>/dev/null`, to tell a kernel problem
 * from a shell one. Every line is written with write() rather than printf, so
 * nothing depends on stdio buffering.
 *
 * Expected: only the lines marked VISIBLE appear. */

#include <fcntl.h>
#include <string.h>
#include <unistd.h>

static void say(int fd, const char *text) { write(fd, text, strlen(text)); }

int main(void) {
	say(1, "VISIBLE   fd 1, before any redirection\n");
	say(2, "VISIBLE   fd 2, before any redirection\n");

	/* The flags a shell uses for `>`, on the device a shell redirects to. */
	int null_fd = open("/dev/null", O_WRONLY | O_CREAT | O_TRUNC, 00644);
	if (null_fd < 0) {
		say(2, "VISIBLE   open(/dev/null) failed\n");
		return 1;
	}

	int saved = dup(2);
	if (saved < 0) {
		say(2, "VISIBLE   dup(2) failed\n");
		return 1;
	}
	if (dup2(null_fd, 2) != 2) {
		say(2, "VISIBLE   dup2(fd, 2) failed\n");
		return 1;
	}
	close(null_fd);

	say(1, "VISIBLE   fd 1, while fd 2 goes to /dev/null\n");
	say(2, "hidden    fd 2, while it goes to /dev/null\n");

	/* And the same through the libc's own panic path, which is what prints
	 * "Stack smashing detected!". It writes to fd 2 without going through
	 * stdio, exactly like the calls above. */
	say(2, "hidden    second write on the redirected fd 2\n");

	dup2(saved, 2);
	close(saved);
	say(2, "VISIBLE   fd 2, once put back\n");
	return 0;
}
