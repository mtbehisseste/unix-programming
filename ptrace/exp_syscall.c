#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <unistd.h>

void errquit(const char *msg)
{
    perror(msg);
    exit(-1);
}

int main(int argc, char *argv[])
{
    pid_t child;
    if (argc < 2) {
        fprintf(stderr, "usage: %s program [args ...]\n", argv[0]);
        return -1;
    }
    if ((child = fork()) < 0)
        errquit("fork");
    if (child == 0) {  // child
        if (ptrace(PTRACE_TRACEME, 0, 0, 0) < 0)
            errquit("ptrace@child");
        execvp(argv[1], argv + 1);
        errquit("execvp");
    } else {  // parent
        long long counter = 0LL;
        int enter = 0x01;
        int wait_status;
        if (waitpid(child, &wait_status, 0) < 0)
            errquit("waitpid");

        // ptrace sig has 0x80 bit marked
        ptrace(PTRACE_SETOPTIONS, child, 0,
               PTRACE_O_EXITKILL |
                   PTRACE_O_TRACESYSGOOD);

        while (WIFSTOPPED(wait_status)) {
            if (ptrace(PTRACE_SYSCALL, child, 0, 0))
                errquit("ptrace@parent");

            // update status of child
            if (waitpid(child, &wait_status, 0) < 0)
                errquit("waitpid");

            // not stop or not syscall
            if (!WIFSTOPPED(wait_status) || !(WSTOPSIG(wait_status) & 0x80))
                continue;

            // a single syscall will reach here twice (enter and exit)
            if (enter)
                counter++;
            enter ^= 0x01;
        }
        fprintf(stderr, "## %lld syscall(s) executed\n", counter);
    }
    return 0;
}
