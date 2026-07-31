/* Empties the terminal and puts the cursor back at the top left. The line
 * discipline passes escape sequences through, so no terminal library is
 * needed for this much. */

#include <unistd.h>

int main(void) {
	static const char sequence[] = "\033[2J\033[H";

	write(1, sequence, sizeof sequence - 1);
	return 0;
}
