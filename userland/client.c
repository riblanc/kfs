#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void	action(int sig)
{
    static int	received = 0;

    if (sig == SIGUSR1)
        ++received;
    else
    {
        printf("%d\n", received);
        exit(0);
    }
}

static void	mt_kill(int pid, char *str)
{
    int		i;
    char	c;

    while (*str)
    {
        i = 8;
        c = *str++;
        while (i--)
        {
            if (c >> i & 1)
                kill(pid, SIGUSR2);
            else
                kill(pid, SIGUSR1);
            usleep(100);
        }
    }
    i = 8;
    while (i--)
    {
        kill(pid, SIGUSR1);
        usleep(100);
    }
}

int	main(int argc, char **argv)
{
    if (argc != 3 || !strlen(argv[2]))
        return (1);
    /* Unbuffered: the handler prints as well, and "Received: " has no newline
     * to flush the line on. */
    setbuf(stdout, NULL);
    printf("Sent    : %zu\n", strlen(argv[2]));
    printf("Received: ");
    signal(SIGUSR1, action);
    signal(SIGUSR2, action);
    mt_kill(atoi(argv[1]), argv[2]);
    while (1)
        pause();
    return (0);
}
