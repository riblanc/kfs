#include <unistd.h>

int main() {
    write(STDOUT_FILENO, "\033[2J\033[H", 7);
    return 0;
}