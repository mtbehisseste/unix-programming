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

#define LIBC "/lib/x86_64-linux-gnu/libc-2.27.so"

// force output to /dev/tty
#define PRINT_TTY(fmt, ...)                                                          \
    void *handle = dlopen(LIBC, RTLD_LAZY);                                          \
    FILE *(*old_fopen)(const char *path, const char *mode) = dlsym(handle, "fopen"); \
    FILE *tty = old_fopen("/dev/tty", "a+");                                         \
    fprintf(tty, fmt, __VA_ARGS__);

// paramName is the parameters with only names, paramDecl is the parameters with
// types and names
#define INJ_FUNC(func, symbol, retType, defaultRetVal, paramName, paramDecl...) \
    retType func(paramDecl)                                                     \
    {                                                                           \
        if(!isSubDir(#func, path))                                              \
            return defaultRetVal;                                               \
        static retType (*old_##func)(paramDecl) = NULL;                         \
        void *handle = dlopen(LIBC, RTLD_LAZY);                                 \
        if (handle) {                                                           \
            old_##func = dlsym(handle, symbol);                                 \
        } else {                                                                \
            PRINT_TTY("[Sandbox] %s: Opening C library error\n",  #func);       \
        }                                                                       \
        return old_##func paramName;                                            \
    }                                                                          

// deal with functions with two paths
#define INJ_FUNC_2PATH(func, symbol, retType, defaultRetVal, paramName, paramDecl...) \
    retType func(paramDecl)                                                           \
    {                                                                                 \
        if(!isSubDir(#func, path) || !isSubDir(#func, newpath))                       \
            return defaultRetVal;                                                     \
        static retType (*old_##func)(paramDecl) = NULL;                               \
        void *handle = dlopen(LIBC, RTLD_LAZY);                                       \
        if (handle) {                                                                 \
            old_##func = dlsym(handle, symbol);                                       \
        } else {                                                                      \
            PRINT_TTY("[Sandbox] %s: Opening C library error\n",  #func);             \
        }                                                                             \
        return old_##func paramName;                                                  \
    }                                                                          

// all functions in exec family are always not allowed
#define RESTRICT_FUNC(func, retType, defaultRetVal, paramName, param...) \
    retType func(param)                                                  \
    {                                                                    \
        PRINT_TTY("[Sandbox] %s(%s): not allowed\n", #func, path);       \
        return defaultRetVal;                                            \
    }

int isSubDir(char* func, const char *path, ...)
{
    char *basedir = getenv("BASEDIR");
    char *absPath = NULL;

    absPath = realpath(path, absPath);
    // path not found or not a subdirectory
    if (!absPath || strncmp(basedir, absPath, strlen(basedir)) != 0) {
        PRINT_TTY("[Sandbox] %s: access to %s is not allowed\n", func, path);
        free(absPath);
        return 0;
    }
    return 404;
}

INJ_FUNC(chdir, "chdir", int, -1, (path), const char *path);
INJ_FUNC(chmod, "chmod", int, -1, (path, mode), const char *path, mode_t mode);
INJ_FUNC(chown, "chown", int, -1, (path, owner, group), const char *path, uid_t owner, gid_t group);
INJ_FUNC(creat, "creat", int, -1, (path, mode), const char *path, mode_t mode);
INJ_FUNC(fopen, "fopen", FILE *, NULL, (path, mode), const char *path, const char *mode);
INJ_FUNC(mkdir, "mkdir", int, -1, (path, mode), const char *path, mode_t mode);
INJ_FUNC(open, "open", int, -1, (path, flags), const char *path, int flags);
INJ_FUNC(openat, "openat", int, -1, (dirfd, path, flags), int dirfd, const char *path, int flags);
INJ_FUNC(opendir, "opendir", DIR *, NULL, (path), const char *path);
INJ_FUNC(readlink, "readlink", ssize_t, -1, (path, buf, bufsize), const char *path, char *buf, size_t bufsize);
INJ_FUNC(remove, "remove", int, -1, (path), const char *path);
INJ_FUNC(rmdir, "rmdir", int, -1, (path), const char *path);
INJ_FUNC(symlink, "symlink", int, -1, (path, linkpath), const char *path, const char *linkpath);
INJ_FUNC(unlink, "unlink", int, -1, (path), const char *path);

// the implementation of `stat` is `__xstat` in libc
INJ_FUNC(__xstat, "__xstat", int, -1, (ver, path, statbuf), int ver, const char *path, struct stat *statbuf);

// deal with the two paths functions
INJ_FUNC_2PATH(rename, "rename", int, -1, (path, newpath), const char *path, const char *newpath);
INJ_FUNC_2PATH(link, "link", int, -1, (path, newpath), const char *path, const char *newpath);

// symbols when large files are supported
INJ_FUNC(creat64, "creat64", int, -1, (path, mode), const char *path, mode_t mode);
INJ_FUNC(fopen64, "fopen64", FILE *, NULL, (path, mode), const char *path, const char *mode);
INJ_FUNC(open64, "open64", int, -1, (path, flags), const char *path, int flags);
INJ_FUNC(openat64, "openat64", int, -1, (dirfd, path, flags), int dirfd, const char *path, int flags);
INJ_FUNC(__xstat64, "__xstat64", int, -1, (ver, path, statbuf), int ver, const char *path, struct stat *statbuf);

// exec family
RESTRICT_FUNC(execl, int, -1, (path, arg), const char *path, const char *arg, ...);
RESTRICT_FUNC(execle, int, -1, (path, arg), const char *path, const char *arg, ...);
RESTRICT_FUNC(execlp, int, -1, (path, arg), const char *path, const char *arg, ...);
RESTRICT_FUNC(execv, int, -1, (path, argv), const char *path, char *const argv[]); 
RESTRICT_FUNC(execvp, int, -1, (path, argv), const char *path, char *const argv[]);
RESTRICT_FUNC(execve, int, -1, (path, argv, envp), const char *path, char *const argv[], char *const envp[]);
RESTRICT_FUNC(system, int, -1, (path), const char *path);

