#include <arpa/inet.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "handler.h"

// reads the given file content
void readFile(char *fileName, char *filterStr)
{
    FILE *fileFd = fopen(fileName, "r");
    if (!fileFd) {
        fprintf(stderr, "Error opening: %s\n", fileName);
        exit(-1);
    }

    // if line is NULL and len is 0, `getline` will allocates a buffer itself
    char *line = NULL;
    size_t len = 0;
    ssize_t r;

    // strip the first line of /proc/net/tcp which is the name of each column
    r = getline(&line, &len, fileFd);

    int i;
    char *col;  // stores tmp columns in line
    char delim[] = " :";
    char *lineArr[30];  // a line should contain less than 20 columns

    char *localAddr, *localAddrPort, *foreignAddr, *foreignAddrPort, *state,
        *PID;
    long inode;
    bool isTCP = (fileName[10] == 't');
    bool isIpv6 = (fileName[strlen(fileName) - 1] == '6');

    while ((r = getline(&line, &len, fileFd)) != -1) {
        i = 0;
        lineArr[i++] = strtok(line, delim);
        // NOTE: should use NULL for the same string
        while ((col = strtok(NULL, delim)))
            // get columns in the line and store in array
            lineArr[i++] = col;

        localAddr = lineArr[1];
        localAddrPort = lineArr[2];
        foreignAddr = lineArr[3];
        foreignAddrPort = lineArr[4];
        state = lineArr[5];
        inode = strtol(lineArr[13], NULL, 10);

        // destination for parsed ipv4 address
        char *localIpDst =
            calloc(isIpv6 ? INET6_ADDRSTRLEN : INET_ADDRSTRLEN, sizeof(char *));
        // destination for parsed ipb6 address
        char *foreignIpDst =
            calloc(isIpv6 ? INET6_ADDRSTRLEN : INET_ADDRSTRLEN, sizeof(char *));
        long localPortInt, foreignPortInt;

        parseIP(localAddr, &localIpDst, isIpv6);
        localPortInt = strtol(localAddrPort, NULL, 16);
        parseIP(foreignAddr, &foreignIpDst, isIpv6);
        foreignPortInt = strtol(foreignAddrPort, NULL, 16);

        // traverse /proc/{pid}/fd to find the PID corresponding to the inode
        PID = findPID(inode);

        // find program name using PID
        char *processName;
        processName = findProgram(PID);

        // print if no filter string given or there's match
        if (!filterStr || isSubStr(processName, filterStr))
            printResult(isTCP, isIpv6, localIpDst, localPortInt, foreignIpDst,
                        foreignPortInt, PID, processName);

        free(localIpDst);
        free(foreignIpDst);
    }

    fclose(fileFd);
    if (line)  // free the allocated line memory
        free(line);
}

void parseIP(char *ip, char **dst, bool isIpv6)
{
    int addressFamily;
    if (!isIpv6) {  // ipv4
        addressFamily = AF_INET;
        struct in_addr *addr = malloc(sizeof(struct in_addr));
        addr->s_addr = (uint32_t) strtol(ip, NULL, 16);
        inet_ntop(addressFamily, addr, *dst, INET_ADDRSTRLEN);
    } else {  // ipv6
        addressFamily = AF_INET6;
        int littleEndianIP[16];
        // cut ip string to array in order to convert to little endian
        for (int j = 0; j < 16; j++) {
            char *tmp = malloc(3);
            strncpy(tmp, ip + j * 2, 2);
            tmp[2] = '\0';
            littleEndianIP[j] = strtol(tmp, NULL, 16);
        }

        // little endian
        struct in6_addr addr = {
            littleEndianIP[3],  littleEndianIP[2],  littleEndianIP[1],
            littleEndianIP[0],  littleEndianIP[7],  littleEndianIP[6],
            littleEndianIP[5],  littleEndianIP[4],  littleEndianIP[11],
            littleEndianIP[10], littleEndianIP[9],  littleEndianIP[8],
            littleEndianIP[15], littleEndianIP[14], littleEndianIP[13],
            littleEndianIP[12]};
        inet_ntop(addressFamily, &addr, *dst, INET6_ADDRSTRLEN);
    }
}

char *findPID(long inode)
{
    DIR *procDirP = opendir("/proc/");  // pointer to the directory
    struct dirent *procDirContent;      // pointer to /proc dir entry
    struct dirent *pidDirContent;       // pointer to /proc/{pid} dir entry

    // traversing all pid
    while ((procDirContent = readdir(procDirP))) {
        if (procDirContent->d_name[0] == '.')  // if match '.' or '..'
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

        char *path =
            calloc(10 + strlen(procDirContent->d_name), sizeof(char *));
        // NOTE: there might be fucking trash in new allocated memory, and will
        // cost you 12hrs to debug this, so remember to initialize memory to 0.
        // Or maybe you should `strcpy(path, "/proc", 5)` first, then `strcat`
        strcat(path, "/proc/");
        strcat(path, procDirContent->d_name);
        strcat(path, "/fd/");
        DIR *pidDirP = opendir(path);  // "/proc/{pid}/fd/"
        if (!pidDirP) {
            // NOTE: errors might be 'no such file or directory' or 'too many
            // open files' so remember to `closedir` when finish

            // fprintf(stderr, "%s %s\n", strerror(errno), path);
            closedir(pidDirP);
            free(path);
            path = NULL;
            continue;
        }

        // traverse all symbol link or directory in this pid (process)
        while ((pidDirContent = readdir(pidDirP))) {
            if (pidDirContent->d_name[0] == '.')
                // if match '.' or '..'
                continue;

            // path to each directory in this pid
            char *p = calloc(strlen(path) + strlen(pidDirContent->d_name),
                             sizeof(char *));
            struct stat *fileStat = malloc(sizeof(struct stat));
            strcat(p, path);
            strcat(p, pidDirContent->d_name);

            if (stat(p, fileStat) != -1) {  // read file states
                if (S_ISSOCK(
                        fileStat->st_mode)) {  // check if the type is socket
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
    return "-";
}

char *findProgram(char *pid)
{
    if (!strcmp(pid, "-"))
        return "-";

    char *path = malloc(strlen("/proc/") + strlen(pid) + strlen("/cmdline"));
    strcpy(path, "/proc/");
    strcat(path, pid);
    strcat(path, "/cmdline");
    FILE *programCmd = fopen(path, "r");
    if (!programCmd) {
        fprintf(stderr, "Error opening: %s\n", path);
        exit(-1);
    }

    // if line is NULL and len is 0, `getline` will allocates a buffer itself
    char *line = NULL;
    size_t len = 0;
    ssize_t r;

    r = getline(&line, &len, programCmd);

    char *program[10], *tmp;
    int i = 0;
    tmp = strtok(line, "/");
    program[++i] = tmp;

    // cut the program path to array and get program name only
    while (tmp && (tmp = strtok(NULL, "/")))
        program[++i] = tmp;

    return program[i];
}

bool isSubStr(char *target, char *pattern)
{
    if (!pattern)
        return true;

    if (strlen(target) < strlen(pattern))
        return false;

    for (int i = 0; i <= strlen(target) - strlen(pattern); i++) {
        if (strncmp(target + i, pattern, strlen(pattern)) == 0)
            return true;
    }
    return false;
}

void printResult(bool isTCP,
                 bool isIpv6,
                 char *localAddr,
                 long localPortInt,
                 char *foreignAddr,
                 long foreignPortInt,
                 char *pid,
                 char *processName)
{
    // change port to '*' if port is 0
    char *localPortStr = calloc(10, sizeof(long));
    char *foreignPortStr = calloc(10, sizeof(long));
    sprintf(localPortStr, localPortInt == 0 ? "*" : "%ld", localPortInt);
    sprintf(foreignPortStr, foreignPortInt == 0 ? "*" : "%ld", foreignPortInt);

    // NOTE: the usage of the format string
    printf("%-5s %s:%-*s %s:%-*s %s/%s\n",
           isTCP ? (isIpv6 ? "tcp6" : "tcp") : (isIpv6 ? "udp6" : "udp"),
           localAddr, 23 - (int) strlen(localAddr), localPortStr, foreignAddr,
           23 - (int) strlen(foreignAddr), foreignPortStr, pid, processName);

    free(localPortStr);
    free(foreignPortStr);
}
