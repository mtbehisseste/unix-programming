# include <stdio.h>
# include <stdlib.h>
# include <getopt.h>
# include <string.h>

# include "handler.h"

// display the usage messages
void usage() {
    printf("usage: ./hw1 [-htu] {fliter-string}\n");
    printf("\n");
    printf("\t-h, --help\tdisplay usage message\n");
    printf("\t-t, --tcp\tdisplay all tcp sockets\n");
    printf("\t-u, --udp\tdisplay all udp sockets\n");
}

int main(int argc, char **argv) {
    // the array of type option to store long options
    static struct option opts[] = { 
        {"help", 0, NULL, 'h'}, 
        {"tcp", 0, NULL, 't'},
        {"udp", 0, NULL, 'u'},
        {0, 0, 0, 0} // TODO: why should this line be added?
    };
    int cmdOpt = 0;
    bool isTCP = false;
    bool isUDP = false;

    // get long options until the end of options
    while ((cmdOpt = getopt_long(argc, argv, "hut", opts, NULL)) != -1) { 
        switch (cmdOpt) {
        case 't':
            isTCP = true;
            break;
        case 'u':
            isUDP = true;
            break;
        case 'h':
            usage();
            break;
        case '?':
        default:
            // NOTE: getopt_long already printed an error message
            usage();
            return -1;
        }
    }    

    char *filterStr = NULL;
    // deal with the arguments after options, i.e. the filter string
    if (argc > optind) { 
        filterStr = malloc(256);
        memset(filterStr, 0, 256);
        strcat(filterStr, argv[optind++]); 
        while (optind < argc) {
            strcat(filterStr, " ");
            strcat(filterStr, argv[optind++]); 
        }
    }

    if (!isTCP && !isUDP) { 
        // both -t and -u is not specified, do both
        printf("List of TCP connections:\n");
        printf("%-5s %-24s %-24s %s\n", "Proto", "Local Address", "Foreign Address", "PID/Program name and arguments");
        readFile("/proc/net/tcp", filterStr);
        readFile("/proc/net/tcp6", filterStr);
        printf("\n");
        printf("List of UDP connections:\n");
        printf("%-5s %-24s %-24s %s\n", "Proto", "Local Address", "Foreign Address", "PID/Program name and arguments");
        readFile("/proc/net/udp", filterStr);
        readFile("/proc/net/udp6", filterStr);
        printf("\n");
    }

    if (isTCP) {
        printf("List of TCP connections:\n");
        printf("%-5s %-24s %-24s %s\n", "Proto", "Local Address", "Foreign Address", "PID/Program name and arguments");
        readFile("/proc/net/tcp", filterStr);
        readFile("/proc/net/tcp6", filterStr);
        printf("\n");
    }
    if (isUDP) {
        printf("List of UDP connections:\n");
        printf("%-5s %-24s %-24s %s\n", "Proto", "Local Address", "Foreign Address", "PID/Program name and arguments");
        readFile("/proc/net/udp", filterStr);
        readFile("/proc/net/udp6", filterStr);
        printf("\n");
    }

    free(filterStr);
    return 0;
}

