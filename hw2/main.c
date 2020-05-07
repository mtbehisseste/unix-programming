/*
 * This program is the interface of the sandbox, which provides the command line prompts.
 * It will be compiled as the binary "sandbox".
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>

// display usage messages
void usage()
{
    printf("usage: ./sandbox [-p sopath] [-d basedir] [--] cmd [cmd args ...]\n"
           "\t-p: set the path to sandbox.so, default = ./sandbox.so\n"
           "\t-d: the base directory that is allowed to access, default = .\n"
           "\t--: separate the arguments for sandbox and for the executed command\n");
}

int main(int argc, char **argv) 
{
    // handle the options
    int cmdOpt = 0;
    while ((cmdOpt = getopt(argc, argv, "p:d:")) != -1) {
        switch (cmdOpt) {
        case 'p':
            break;
        case 'd':
            break;
        case '?':
        default:
            usage();
            return -1;
        }
    }

    // handle the commands
    char *command = NULL;
    if (argc > optind) {
        command = calloc(256, sizeof (char *));
        strcat(command, argv[optind]);
        if (optind + 1 < argc - 1)
            optind++;
    }
    else {
        fprintf(stderr, "no command given.\n");
        return -1;
    }
    execvp(command, argv + optind);

    return 0;
}
