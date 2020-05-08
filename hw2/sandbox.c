/*
 * This program is the library to be injected.
 * It will be compiled as shared library "sandbox.so"
 */

#include <dirent.h>
#include <dlfcn.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

// paramName is the parameters with only names, paramDecl is the parameters with
// types and names
#define INJ_FUNC(func, retType, defaultRetVal, paramName,  paramDecl...)        \
    retType func(paramDecl)                                                     \
    {                                                                           \
        int isSub = isSubDir(#func, path);                                      \
        if (!isSub)                                                             \
            return defaultRetVal;                                               \
        static retType (*old_##func)(paramDecl) = NULL;                         \
        void *handle = dlopen("/lib/x86_64-linux-gnu/libc-2.27.so", RTLD_LAZY); \
        if (handle) {                                                           \
            old_##func = dlsym(handle, #func);                                  \
        } else {                                                                \
            fprintf(stderr, "[Sandbox] %s: Opening C library error\n",  #func); \
        }                                                                       \
        return old_##func paramName;                                            \
    }                                                                          

#define RESTRICT_FUNC(func, retType, defaultRetVal, paramName, param...) \
    retType func(param)                                                  \
    {                                                                    \
        fprintf(stderr, "[Sandbox] %s: not allowed\n", #func);           \
        return defaultRetVal;                                            \
    }

int isSubDir(char* func, const char *path) 
{
    char *basedir = getenv("BASEDIR");
    char *absPath = NULL;

    absPath = realpath(path, absPath);
    // path not found or not a subdirectory
    if (!absPath || strncmp(basedir, absPath, strlen(basedir)) > 0) {
        fprintf(stderr, "[Sandbox] %s: access to %s is not allowed\n", func, path);
        free(absPath);
        return 0;
    }
    return 404;
}

INJ_FUNC(chdir, int, -1, (path), const char *path);
INJ_FUNC(chmod, int, -1, (path, mode), const char *path, mode_t mode);
INJ_FUNC(chown, int, -1, (path, owner, group), const char *path, uid_t owner, gid_t group);
INJ_FUNC(creat, int, -1, (path, mode), const char *path, mode_t mode);
INJ_FUNC(fopen, FILE *, NULL, (path, mode), const char *path, const char *mode);
INJ_FUNC(mkdir, int, -1, (path, mode), const char *path, mode_t mode);
INJ_FUNC(open, int, -1, (path, flags), const char *path, int flags);
INJ_FUNC(openat, int, -1, (dirfd, path, flags), int dirfd, const char *path, int flags);
INJ_FUNC(opendir, DIR *, NULL, (path), const char *path);
INJ_FUNC(readlink, ssize_t, -1, (path, buf, bufsize), const char *path, char *buf, size_t bufsize);
INJ_FUNC(remove, int, -1, (path), const char *path);
INJ_FUNC(rmdir, int, -1, (path), const char *path);
INJ_FUNC(stat, int, -1, (path, statbuf), const char *path, struct stat *statbuf);
INJ_FUNC(symlink, int, -1, (path, linkpath), const char *path, const char *linkpath);
INJ_FUNC(unlink, int, -1, (path), const char *path);

// TODO deals with the two paths functions
/* INJ_FUNC(rename, int, -1, "s s", (oldpath, newpath), const char *oldpath, const char *newpath); */
/* INJ_FUNC(link, int, -1, "ss", (oldpath, newpath), const char *oldpath, const char *newpath); */

// TODO make sure the symbols in libc of all functions 
RESTRICT_FUNC(execl, int, -1, (path, arg), const char *path, const char *arg, ...);
RESTRICT_FUNC(execle, int, -1, (path, arg), const char *path, const char *arg, ...);
RESTRICT_FUNC(execlp, int, -1, (path, arg), const char *path, const char *arg, ...);
RESTRICT_FUNC(execv, int, -1, (path, argv), const char *path, char *const argv[]); 
RESTRICT_FUNC(execvp, int, -1, (path, argv), const char *path, char *const argv[]);
RESTRICT_FUNC(execve, int, -1, (path, argv, envp), const char *path, char *const argv[], char *const envp[]);
RESTRICT_FUNC(system, int, -1, command, const char *command);

// TODO handle error message
