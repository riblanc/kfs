/* Exercises the signal sysdeps: signal, sigaction, raise and the two ways a
 * request can be refused. */

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>

static volatile sig_atomic_t caught;
static volatile sig_atomic_t caught_signo;

static void handler(int sn) { caught = sn; }

static void info_handler(int sn, siginfo_t *si, void *ctx) {
	(void)ctx;
	caught = sn;
	caught_signo = si->si_signo;
}

static int failures;

static void check(const char *what, int ok) {
	printf("%-40s %s\n", what, ok ? "ok" : "FAILED");
	if (!ok)
		failures++;
}

int main(void) {
	caught = 0;
	check("signal(SIGUSR1) installs", signal(SIGUSR1, handler) != SIG_ERR);
	check("raise(SIGUSR1) reaches the handler", raise(SIGUSR1) == 0 && caught == SIGUSR1);
	check("signal() hands back the old handler", signal(SIGUSR1, SIG_DFL) == handler);

	/* SA_SIGINFO picks the three argument form, and si_signo has to survive
	 * the trip through the kernel's own struct. */
	struct sigaction sa;
	memset(&sa, 0, sizeof sa);
	sa.sa_sigaction = info_handler;
	sa.sa_flags = SA_SIGINFO;
	sigemptyset(&sa.sa_mask);
	caught = caught_signo = 0;
	check("sigaction(SA_SIGINFO) installs", sigaction(SIGUSR2, &sa, NULL) == 0);
	raise(SIGUSR2);
	check("handler received siginfo", caught == SIGUSR2 && caught_signo == SIGUSR2);

	/* Neither of these may take the kernel down. */
	errno = 0;
	check("signal(SIGKILL) refused with EINVAL", signal(SIGKILL, handler) == SIG_ERR && errno == EINVAL);
	errno = 0;
	check("signal(4242) refused with EINVAL", signal(4242, handler) == SIG_ERR && errno == EINVAL);

	/* A refused call must not have disturbed what was already installed. */
	struct sigaction old;
	memset(&old, 0, sizeof old);
	check("sigaction reads back SA_SIGINFO", sigaction(SIGUSR2, NULL, &old) == 0 &&
			old.sa_sigaction == info_handler && (old.sa_flags & SA_SIGINFO));

	printf("\n%s\n", failures ? "some checks failed" : "all checks passed");
	return failures != 0;
}
