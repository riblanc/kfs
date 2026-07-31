/* Stand-in for GNU readline's history, which shadokos does not have.
 * Lines are not kept, so this records nothing. */

#ifndef HISTORY_H
#define HISTORY_H

void add_history(const char *line);
void clear_history(void);

#endif /* HISTORY_H */
