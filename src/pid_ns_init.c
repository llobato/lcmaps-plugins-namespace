
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

long int child_pid;

void signal_handler(int signo)
{
    if (child_pid > 0)
    {
        kill(child_pid, signo);
    }
}

int main(int argc, char **argv)
{
    child_pid = -1;

    if (argc != 2)
    {
        return 127;
    }

    // Install signal handler so we can pass signals to the children.
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    sigaction(SIGHUP, &sa, 0);
    sigaction(SIGINT, &sa, 0);
    sigaction(SIGQUIT, &sa, 0);
    sigaction(SIGILL, &sa, 0);
    sigaction(SIGABRT, &sa, 0);
    sigaction(SIGTERM, &sa, 0);
    sigaction(SIGSEGV, &sa, 0);
    sigaction(SIGPIPE, &sa, 0);
    sigaction(SIGUSR1, &sa, 0);
    sigaction(SIGUSR2, &sa, 0);

    // Get the child PID from the parent
    errno = 0;
    child_pid = strtol(argv[0], NULL, 10);
    if (errno)
    {
        return 127;
    }

    // Reap children, pass along signals, exit correctly.
    int status;
    while (1)
    {
        int result = wait(&status);
        if (((result == -1) && (errno == EINTR)) || (result != child_pid))
        {
            continue;
        }
        _exit(status);
    }
}
