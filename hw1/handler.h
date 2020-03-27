# include <stdbool.h>

/* reads the file content */
void readFile(char*, char*);

/* parse IP address to human readable */
void parseIP(char*, char**, bool); 

/* find PID corresponding to the inode */
char *findPID(long);

/* find program using given PID */
char *findProgram(char*);

/* check if `pattern` is substring of `target` */
bool isSubStr(char *target, char *pattern);

/* print the result */
void printResult(bool isTCP, bool isIpv6, char *localAddr, long localPortInt, 
        char *foreignAddr, long foreignPortInt, char *pid, char* processName);
