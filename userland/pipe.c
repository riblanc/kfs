/* Exercises pipe(). The two cases that matter are the ones the reference count
 * exists for: a read that ends because the last writer left, and a write that
 * fails because the last reader did. */

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static int failures;
static volatile sig_atomic_t got_sigpipe;

static void on_sigpipe(int sn) { got_sigpipe = sn; }

static void check(const char *what, int ok) {
	printf("%-46s %s\n", what, ok ? "ok" : "FAILED");
	if (!ok)
		failures++;
}

int main(void) {
	setbuf(stdout, NULL);

	int fds[2];
	check("pipe() succeeds", pipe(fds) == 0);
	check("the two ends differ", fds[0] != fds[1]);

	const char msg[] = "through the pipe";
	check("write into the pipe", write(fds[1], msg, sizeof msg) == (ssize_t)sizeof msg);

	char buffer[64];
	memset(buffer, 0, sizeof buffer);
	check("read what was written", read(fds[0], buffer, sizeof buffer) == (ssize_t)sizeof msg);
	check("the bytes came back unchanged", strcmp(buffer, msg) == 0);

	/* Each end refuses the direction it is not. */
	errno = 0;
	check("writing to the read end fails", write(fds[0], msg, 1) < 0);
	errno = 0;
	check("reading from the write end fails", read(fds[1], buffer, 1) < 0);

	/* With the last writer gone, a read stops instead of waiting. */
	check("close the write end", close(fds[1]) == 0);
	check("read then reports end of file", read(fds[0], buffer, sizeof buffer) == 0);
	check("close the read end", close(fds[0]) == 0);

	/* And with the last reader gone, a write is refused twice over. */
	check("a second pipe", pipe(fds) == 0);
	signal(SIGPIPE, on_sigpipe);
	check("close its read end", close(fds[0]) == 0);
	errno = 0;
	got_sigpipe = 0;
	ssize_t n = write(fds[1], msg, sizeof msg);
	check("write fails with EPIPE", n < 0 && errno == EPIPE);
	check("and raised SIGPIPE", got_sigpipe == SIGPIPE);
	check("close its write end", close(fds[1]) == 0);

	printf("\n%s\n", failures ? "some checks failed" : "all checks passed");
	return failures != 0;
}
