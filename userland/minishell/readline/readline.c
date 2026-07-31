#include "readline.h"
#include "history.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int rl_done;
FILE *rl_outstream;
int (*rl_event_hook)(void);

/* Grown as the line does: a terminal in canonical mode returns at most one line
 * per read, but says nothing about how long it is. */
static char *append(char *line, size_t *capacity, size_t length, char c) {
	if (length + 2 > *capacity) {
		size_t wanted = *capacity ? *capacity * 2 : 64;
		char *bigger = realloc(line, wanted);
		if (!bigger) {
			free(line);
			return NULL;
		}
		line = bigger;
		*capacity = wanted;
	}
	line[length] = c;
	return line;
}

char *readline(const char *prompt) {
	char *line = NULL;
	size_t capacity = 0;
	size_t length = 0;

	rl_done = 0;
	if (prompt) {
		FILE *out = rl_outstream ? rl_outstream : stderr;
		fputs(prompt, out);
		fflush(out);
	}

	for (;;) {
		char c;
		ssize_t n = read(0, &c, 1);

		/* A signal cut the read short. The handler sets rl_done, and the
		 * caller wants an empty line rather than the end of input. */
		if (n < 0 && errno == EINTR) {
			if (rl_done)
				break;
			continue;
		}
		if (n <= 0) {
			/* Nothing read at all is the end of input; a partial line is
			 * still worth returning. */
			if (length == 0) {
				free(line);
				return NULL;
			}
			break;
		}
		if (c == '\n')
			break;
		line = append(line, &capacity, length, c);
		if (!line)
			return NULL;
		++length;
		if (rl_done)
			break;
	}

	/* The terminal echoed ^C but knows nothing of the line being abandoned.
	 * Ending it is the shell's job, and here that means this. */
	if (rl_done)
		write(1, "\n", 1);

	line = append(line, &capacity, length, '\0');
	if (!line)
		return NULL;
	line[length] = '\0';
	return line;
}

void add_history(const char *line) { (void)line; }

void clear_history(void) {}
