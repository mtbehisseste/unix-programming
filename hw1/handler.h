# include <stdbool.h>

/* reads the file content */
void readFile(char*);

/* parse IP address to human readable */
void parseIP(char*, char**, bool); 

/* find PID corresponding to the inode */
char *findPID(long);

/* find program using given PID */
char *findProgram(char*);

/* print the result */
void printResult(bool isTCP, bool isIpv6, char *localAddr, int localPortInt, 
        char *foreignAddr, int foreignPortInt, char *pid, char* processName);
