# include <stdio.h>
# include <stdlib.h>
# include <stdbool.h>
# include <stdint.h>
# include <unistd.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <dirent.h>
# include <string.h>
# include <errno.h>
# include <ctype.h>
# include <arpa/inet.h>
# include <netinet/in.h>

# include "handler.h"

/* reads the given file content */
void readFile(char *fileName) {
    FILE *fileFd = fopen(fileName, "r");
    if (!fileFd) {
        fprintf(stderr, "Error opening: %s\n", fileName);
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

    char *localAddr, *localAddrPort, *foreignAddr, *foreignAddrPort, *state, *PID;
    long inode;
    bool isTCP = (fileName[10] == 't'); // check whether TCP or UDP
    bool isIpv6 = (fileName[strlen(fileName) - 1] == '6'); // check if it's ipv6

    while ((r = getline(&line, &len, fileFd)) != -1) { // get the line in /proc/net/tcp
        i = 0;
        lineArr[i++] = strtok(line, delim); 
        while ((col = strtok(NULL, delim))) // should use NULL for the same string
            lineArr[i++] = col; // get columns in the line and store in array

        localAddr = lineArr[1];
        localAddrPort = lineArr[2];
        foreignAddr = lineArr[3];
        foreignAddrPort = lineArr[4];
        state = lineArr[5];
        inode = strtol(lineArr[13], NULL, 10);

        // parse ip address
        char *localIpDst = malloc(isIpv6 ? INET6_ADDRSTRLEN : INET_ADDRSTRLEN); // destination for parsed ipv4 address
        char *foreignIpDst = malloc(isIpv6 ? INET6_ADDRSTRLEN : INET_ADDRSTRLEN); // destination for parsed ipb6 address
        int localPortInt, foreignPortInt;
        parseIP(localAddr, &localIpDst, isIpv6);
        localPortInt = (int)strtol(localAddrPort, NULL, 16);
        parseIP(foreignAddr, &foreignIpDst, isIpv6);
        foreignPortInt = (int)strtol(foreignAddrPort, NULL, 16);

        // traverse /proc/{pid}/fd to find the PID corresponding to the inode
        PID = findPID(inode);
        
        // find program name using PID
        char *processName = findProgram(PID); 

        // output
        printResult(isTCP, isIpv6, localIpDst, localPortInt, foreignIpDst, foreignPortInt, PID, processName);
    }
    
    fclose(fileFd);
    if (line) // free the allocated line memory
        free(line);
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
        struct in6_addr addr = {littleEndianIP[3], littleEndianIP[2], littleEndianIP[1], littleEndianIP[0],
            littleEndianIP[7], littleEndianIP[6], littleEndianIP[5], littleEndianIP[4],
            littleEndianIP[11], littleEndianIP[10], littleEndianIP[9], littleEndianIP[8],
            littleEndianIP[15], littleEndianIP[14], littleEndianIP[13], littleEndianIP[12]}; // little endian
        inet_ntop(addressFamily, &addr, *dst, INET6_ADDRSTRLEN);
    }
}

char *findPID(long inode) {
    DIR *procDirP = opendir("/proc/"); // pointer to the directory
    struct dirent *procDirContent; // pointer to /proc dir entry
    struct dirent *pidDirContent; // pointer to /proc/{pid} dir entry

    while ((procDirContent = readdir(procDirP))) { // traverse all pid
        if (procDirContent->d_name[0] == '.') // if match '.' or '..'
            continue;
        
        // make sure traverse only pid directory in /proc
        bool dirNameIsDigit = true;
        for (int i = 0; i < strlen(procDirContent->d_name); i++) {
            if (!isdigit(procDirContent->d_name[i])) {
                dirNameIsDigit = false;
                break;
            }
        }
        if (!dirNameIsDigit) 
            continue;

        char *path = malloc(9 + strlen(procDirContent->d_name));
        // NOTE: there might be fucking trash in new allocated memory, and will cost you 12hrs to debug this QQ
        // Or maybe you should `strcpy(path, "/proc", 5)` first then `strcat`
        memset(path, 0, 9 + strlen(procDirContent->d_name)); 
        strcat(path, "/proc/");
        strcat(path, procDirContent->d_name); 
        strcat(path, "/fd/");
        DIR *pidDirP = opendir(path); // "/proc/{pid}/fd/"
        if (!pidDirP) { 
            // NOTE: errors might be 'no such file or directory' or 'too many open files'
            // so remember to `closedir` when finish
            
            // fprintf(stderr, "%s %s\n", strerror(errno), path);
            closedir(pidDirP);
            free(path);
            path = NULL;
            continue;
        }

        while ((pidDirContent = readdir(pidDirP))) { // traverse all symbol link or directory in this pid (process)
            if (pidDirContent->d_name[0] == '.') // if match '.' or '..'
                continue;
            
            // printf("%s %s\n", procDirContent->d_name, pidDirContent->d_name);
            char *p = malloc(strlen(path) + strlen(pidDirContent->d_name)); // path to each directory in this pid
            memset(p, 0, strlen(path) + strlen(pidDirContent->d_name));
            struct stat *fileStat = malloc(sizeof(struct stat));
            strcat(p, path);
            strcat(p, pidDirContent->d_name);    
            if (stat(p, fileStat) != -1) { // read file states
                if (S_ISSOCK(fileStat->st_mode)) { // check if the type is socket
                    if (fileStat->st_ino == inode) {
                        free(path);
                        free(p);
                        free(fileStat);
                        closedir(procDirP);
                        return procDirContent->d_name;
                    }
                }
            }
            free(fileStat);
            free(p);
        }
        closedir(pidDirP);
        free(path);
    }
    closedir(procDirP);
}

char *findProgram(char *pid) {
    char *path = malloc(strlen("/proc/") + strlen(pid) + strlen("/cmdline")); 
    strcpy(path, "/proc/");
    strcat(path, pid);
    strcat(path, "/cmdline");
    FILE *programCmd = fopen(path, "r");
    if (!programCmd) {
        fprintf(stderr, "Error opening: %s\n", path);
        exit(-1);
    }

    char *line = NULL;
    size_t len = 0; // if line is NULL and len is 0, `getline` will allocates a buffer itself
    ssize_t r;
    r = getline(&line, &len, programCmd);  

    char *program[10];
    char *tmp;
    int i = 0;
    tmp = strtok(line, "/");
    program[++i] = tmp;
    while(tmp && (tmp = strtok(NULL, "/"))) 
        program[++i] = tmp;

    return program[i];
}

void printResult(bool isTCP, bool isIpv6,
        char *localAddr, int localPortInt, 
        char *foreignAddr, int foreignPortInt,
        char *pid, char* processName) {
    // TODO: change 0 to * if port is 0
    printf("%-5s %s:%-*d %s:%-*d %s/%s\n", isTCP ? (isIpv6 ? "tcp6" : "tcp") : (isIpv6 ? "udp6" : "udp"),
                                    localAddr, 23 - (int)strlen(localAddr), localPortInt, 
                                    foreignAddr, 23 - (int)strlen(foreignAddr), foreignPortInt, 
                                    pid, processName); // NOTE: the usage of the format string
}
