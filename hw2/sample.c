#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>

#define	FILE_A	"/tmp/aaa"
#define	FILE_B	"/tmp/bbb"
#define	FILE_NULL	"/dev/null"

int main() {
	struct stat st;
	char *argv[] = { FILE_A, NULL };
	char buf[128];
	chdir(".");
	chmod(FILE_A, 0644);
	chown(FILE_A, 0, 0);
	creat(FILE_A, O_RDONLY);
	fopen(FILE_A, "rt");
	link(FILE_A, FILE_B);
	mkdir(FILE_A, 0755);
	open(FILE_A, O_RDONLY);
	openat(AT_FDCWD, FILE_A, 0755);
	opendir(FILE_A);
	readlink(FILE_NULL, buf, sizeof(buf));
	remove(FILE_A);
	rename(FILE_A, FILE_B);
	rmdir(FILE_A);
	stat(FILE_NULL, &st);
	symlink(FILE_A, FILE_B);
	unlink(FILE_A);
	execl(FILE_A, FILE_A, NULL);
	execle(FILE_A, FILE_A, NULL, NULL);
	execlp(FILE_A, FILE_A, NULL);
	execv(FILE_A, argv);
	execvp(FILE_A, argv);
	execve(FILE_A, argv, NULL);
	system("echo -n");
	return -1;
}

