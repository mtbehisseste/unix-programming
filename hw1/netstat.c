# include <stdio.h>
# include <getopt.h>

# include "handler.h"

/* display the usage messages */
void usage() {
    printf("usage: ./hw1 [-htu] {fliter-string}\n");
    printf("\n");
    printf("\t-h, --help\tdisplay usage message\n");
    printf("\t-t, --tcp\tdisplay all tcp sockets\n");
    printf("\t-u, --udp\tdisplay all udp sockets\n");
}

int main(int argc, char **argv) {
    static struct option opts[] = { // the array of type option to store long options
        {"help", 0, NULL, 'h'}, 
        {"tcp", 0, NULL, 't'},
        {"udp", 0, NULL, 'u'},
        {0, 0, 0, 0} // TODO: why should this line be added?
    };
    int cmdOpt = 0;

    while ((cmdOpt = getopt_long(argc, argv, "hut", opts, NULL)) != -1) { // get long options until the end of options
        switch (cmdOpt) {
        case 't':
            readFile("/proc/net/tcp");
            /* readFile("/proc/net/tcp6"); */
            break;
        case 'u':
            readFile("/proc/net/udp");
            /* readFile("/proc/net/udp6"); */
            break;
        case 'h':
            usage();
            break;
        case '?':
        default: // TODO: no option should print all results
            // getopt_long already printed an error message
            usage();
            return -1;
        }
    }    

    if (argc > optind) { // deal with the arguments after options, i.e. the filter string
        if (argc - optind != 1) {
            fprintf(stderr, "Invalid number of filter string\n");
            return -1;
        }

        printf("here filter string\n");
    }

    return 0;
}

