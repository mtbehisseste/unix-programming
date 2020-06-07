#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ptrace.h>
#include <sys/stat.h>
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
    int wait_status;
    pid_t child;

    if ((child = fork()) < 0)
        errquit("fork");
    if (child == 0) {  // child
        if (ptrace(PTRACE_TRACEME, 0, 0, 0) < 0)
            errquit("ptrace@child");
        execvp("./no_more_traps", argv + 1);
        errquit("execvp");
    } else {
        // read patched codes from file
        int fd = open("./no_more_traps.txt", O_RDONLY);

        // strip first trap, not quite sure why 0x00 is trapped
        waitpid(child, &wait_status, 0);
        ptrace(PTRACE_CONT, child, 0, 0);

        long code;
        struct user_regs_struct regs;
        while (waitpid(child, &wait_status, 0) > 0) {
            // we only care about SIGTRAP
            if (WSTOPSIG(wait_status) != SIGTRAP)
                continue;

            // get registers to get current rip
            if (ptrace(PTRACE_GETREGS, child, 0, &regs))
                errquit("ptrace(GETREGS)");

            // read opcode and convert to long type
            char *c = calloc(2, sizeof(char));
            read(fd, c, 2);
            unsigned long patch_code;
            sscanf(c, "%lx", &patch_code);

            // patch 0xcc with given opcodes
            code = ptrace(PTRACE_PEEKTEXT, child, regs.rip - 1, 0);
            if (ptrace(PTRACE_POKETEXT, child, regs.rip - 1,
                       (code & 0xffffffffffffff00 | patch_code)))
                errquit("ptrace(POKETEXT)");
            code = ptrace(PTRACE_PEEKTEXT, child, regs.rip - 1, 0);

            // run the patched codes
            regs.rip = regs.rip - 1;
            if (ptrace(PTRACE_SETREGS, child, 0, &regs))
                errquit("ptrace(SETREGS)");
            ptrace(PTRACE_CONT, child, 0, 0);
        }
    }

    return 0;
}
