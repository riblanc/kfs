/* Prints the environment, one variable per line. */

#include <stdio.h>

extern char **environ;

int main(void) {
	for (char **variable = environ; variable && *variable; ++variable)
		printf("%s\n", *variable);
	return 0;
}
