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
#define injFunc(func, retType, defaultRetVal, paramFmt, paramName,           \
                paramDecl...)                                                \
    static retType (*old_##func)(paramDecl) = NULL;                          \
    retType func(paramDecl)                                                  \
    {                                                                        \
        if (!old_##func) {                                                   \
            void *handle =                                                   \
                dlopen("/lib/x86_64-linux-gnu/libc.so.6", RTLD_LAZY);        \
            if (handle)                                                      \
                old_##func = dlsym(handle, #func);                           \
            else                                                             \
                fprintf(stderr, "[Sandbox] %s: Opening C library error",     \
                        #func);                                              \
        }                                                                    \
        if (old_##func) {                                                    \
            fprintf(stderr, "%s ", #func);                                   \
            char *abs = isSubDir(paramFmt, paramName);                       \
            if (abs) {                                                       \
                free(abs);                                                   \
                return old_##func paramName;                                 \
            }                                                                \
            else {                                                           \
                fprintf(stderr, "[Sandbox] %s: access to %s is not allowed", \
                        #func, abs);                                         \
                free(abs);                                                   \
                return defaultRetVal;                                        \
            }                                                                \
        }                                                                    \
    }

#define restrictFunc(func, retType, defaultRetVal, paramName, param...) \
    retType func(param)                                                 \
    {                                                                   \
        fprintf(stderr, "[Sandbox] %s: not allowed\n", #func);          \
        return defaultRetVal;                                           \
    }

// Returns the absolute path of the subdirectory if it is a subdirectory of 
// the base directory, return NULL if not
char *isSubDir(char *fmt, ...)
{
    va_list ap;
    const char *s;

    char *basedir = getenv("BASEDIR");
    char *absPath = NULL;

    va_start(ap, fmt);
    while (*fmt) {
        switch (*fmt++) {
        // since we only care about the pathname, so string is enough
        case 's':
            s = va_arg(ap, char *);
            absPath = realpath(s, absPath);
            printf("s abs: %s %s %s\n", s, absPath, basedir);
            if (absPath && strncmp(basedir, absPath, strlen(basedir)) <= 0)
                return absPath;
            break;
        // we don't care for other types of parameters
        default:
            break;
        }
    }
    va_end(ap);

    return NULL;
}

injFunc(chdir, int, -1, "s", (path), const char *path);
injFunc(chmod, int, -1, "sd", (pathname, mode), const char *pathname, mode_t mode);
injFunc(chown, int, -1, "sdd", (pathname, owner, group), const char *pathname, uid_t owner, gid_t group);
injFunc(creat, int, -1, "sd", (pathname, mode), const char *pathname, mode_t mode);
injFunc(fopen, FILE *, NULL, "sd", (pathname, mode), const char *pathname, const char *mode);
injFunc(link, int, -1, "ss", (oldpath, newpath), const char *oldpath, const char *newpath);
injFunc(mkdir, int, -1, "sd", (pathname, mode), const char *pathname, mode_t mode);
injFunc(open, int, -1, "sd", (pathname, flags), const char *pathname, int flags);
injFunc(openat, int, -1, "dsd", (dirfd, pathname, flags), int dirfd, const char *pathname, int flags);
injFunc(opendir, DIR *, NULL, "s", (name), const char *name);
injFunc(readlink, ssize_t, -1, "sdd", (pathname, buf, bufsize), const char *pathname, char *buf, size_t bufsize);
injFunc(remove, int, -1, "s", (pathname), const char *pathname);
injFunc(rename, int, -1, "s s", (oldpath, newpath), const char *oldpath, const char *newpath);
injFunc(rmdir, int, -1, "s", (pathname), const char *pathname);
injFunc(stat, int, -1, "sd", (pathname, statbuf), const char *pathname, struct stat *statbuf);
injFunc(symlink, int, -1, "ss", (target, linkpath), const char *target, const char *linkpath);
injFunc(unlink, int, -1, "s", (pathname), const char *pathname);

restrictFunc(execl, int, -1, (path, arg), const char *path, const char *arg, ...);
restrictFunc(execle, int, -1, (path, arg), const char *path, const char *arg, ...);
restrictFunc(execlp, int, -1, (path, arg), const char *path, const char *arg, ...);
restrictFunc(execv, int, -1, (path, argv), const char *path, char *const argv[]); 
restrictFunc(execvp, int, -1, (path, argv), const char *path, char *const argv[]);
restrictFunc(execve, int, -1, (path, argv, envp), const char *filename, char *const argv[], char *const envp[]);
restrictFunc(system, int, -1, command, const char *command);

// TODO handle error message
