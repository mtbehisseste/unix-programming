# include <stdio.h>
# include <stdlib.h>
# include <stdbool.h>
# include <stdint.h>
# include <sys/types.h>
# include <dirent.h>
# include <string.h>
# include <arpa/inet.h>
# include <netinet/in.h>

# include "handler.h"

/* reads the given file content */
void readFile(char *fileName) {
    FILE *fileFd = fopen(fileName, "r");
    if (!fileFd) {
        perror("fopen");
        exit(-1);
    }

    char *line = NULL;
    size_t len = 0; // if line is NULL and len is 0, `getline` will allocates a buffer itself
    ssize_t r;
    r = getline(&line, &len, fileFd); // strip the first line of /proc/net/tcp which is the name of each column

    int i;
    char *col; // stores tmp columns in line 
    char delim[] = " :";
    char *lineArr[30]; // a line should contain less than 20 columns

    char *inode, *localAddr, *localAddrPort, *foreignAddr, *foreignAddrPort, *state, *PID;
    bool isIpv6 = (fileName[strlen(fileName) - 1] == '6'); // check if it's ipv6

    while ((r = getline(&line, &len, fileFd)) != -1) { // get the line in /proc/net/tcp
        i = 0;
        lineArr[i++] = strtok(line, delim); // calling `strtok` with same string should use NULL next time
        while ((col = strtok(NULL, delim))) // get columns in the line and store in array
            lineArr[i++] = col;

        localAddr = lineArr[1];
        localAddrPort = lineArr[2];
        foreignAddr = lineArr[3];
        foreignAddrPort = lineArr[4];
        state = lineArr[5];
        inode = lineArr[13];

        // parse ip address
        char *localIpDst = malloc(isIpv6 ? INET6_ADDRSTRLEN : INET_ADDRSTRLEN); // destination for parsed ipv4 address
        char *foreignIpDst = malloc(isIpv6 ? INET6_ADDRSTRLEN : INET_ADDRSTRLEN); // destination for parsed ipb6 address
        int localPortInt, foreignPortInt;
        parseIP(localAddr, &localIpDst, isIpv6);
        localPortInt = (int)strtol(localAddrPort, NULL, 16);
        /* printf("local: %s:%d\n", localIpDst, localPortInt); */
        parseIP(foreignAddr, &foreignIpDst, isIpv6);
        foreignPortInt = (int)strtol(foreignAddrPort, NULL, 16);
        /* printf("foreign: %s:%d\n", foreignIpDst, foreignPortInt); */

        // traverse fd to find pid
        
    }
    
    fclose(fileFd);
    if (line) // free the allocated line memory
        free(line);

    /* below should be seperated to anther file  */
    /* DIR *pidDirP = opendir("/proc/1011/fd"); // pointer to the directory */
    /* struct dirent *pidDirContent; // pointer to dir entry */

    /* while ((tcpDirContent = readdir(pidDirP))) { */
    /*     printf("d_name: %s\n", tcpDirContent->d_name); */
    /* } */
}

void parseIP(char *ip, char **dst, bool isIpv6) {
    int addressFamily;
    if (!isIpv6) { // ipv4
        addressFamily = AF_INET;
        struct in_addr *addr = malloc(sizeof(struct in_addr));
        addr->s_addr = (uint32_t)strtol(ip, NULL, 16);
        inet_ntop(addressFamily, addr, *dst, INET_ADDRSTRLEN);
    } else { // ipv6
        addressFamily = AF_INET6;
        int littleEndianIP[16];
        for (int j = 0; j < 16; j++) { // cut ip string to array in order to convert to little endian
            char *tmp = malloc(2); 
            strncpy(tmp, ip + j * 2, 2);
            littleEndianIP[j] = strtol(tmp, NULL, 16);
        }
        // little endian order
        struct in6_addr addr = {littleEndianIP[3], littleEndianIP[2], littleEndianIP[1], littleEndianIP[0],
            littleEndianIP[7], littleEndianIP[6], littleEndianIP[5], littleEndianIP[4],
            littleEndianIP[11], littleEndianIP[10], littleEndianIP[9], littleEndianIP[8],
            littleEndianIP[15], littleEndianIP[14], littleEndianIP[13], littleEndianIP[12]};
        inet_ntop(addressFamily, &addr, *dst, INET6_ADDRSTRLEN);
    }
}
