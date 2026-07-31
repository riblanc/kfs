/* Stand-in for GNU readline, which shadokos does not have.
 *
 * Only what this shell touches is declared. Reading a line is the terminal's
 * job here: in canonical mode the line discipline already handles erasing and
 * hands over a whole line, so nothing is left for a library to do. What is lost
 * is history and moving inside the line.
 *
 * Sitting under this name means the shell's own sources compile unmodified. */

#ifndef READLINE_H
#define READLINE_H

#include <stdio.h>

/* Prints the prompt, then returns a newly allocated line without its newline,
 * or NULL at end of input. */
char *readline(const char *prompt);

/* Set by a SIGINT handler to abandon the line being read. */
extern int rl_done;

/* Where the prompt goes. */
extern FILE *rl_outstream;

/* Called while waiting for input. Never invoked here: a read blocks in the
 * kernel rather than polling. */
extern int (*rl_event_hook)(void);

#endif /* READLINE_H */
