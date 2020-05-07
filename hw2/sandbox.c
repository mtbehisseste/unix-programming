/*
 * This program is the library to be injected.
 * It will be compiled as shared library "sandbox.so"
 */

#include <dirent.h>
#include <dlfcn.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

// paramName is the parameters with only names, paramDecl is the parameters with
// types and names
#define injFunc(func, retType, paramName, paramDecl...)               \
    static retType (*old_##func)(paramDecl) = NULL;                   \
    retType func(paramDecl)                                           \
    {                                                                 \
        if (!old_##func) {                                            \
            void *handle =                                            \
                dlopen("/lib/x86_64-linux-gnu/libc.so.6", RTLD_LAZY); \
            if (handle)                                               \
                old_##func = dlsym(handle, #func);                    \
            else                                                      \
                fprintf(stderr, "Opening C library error");           \
        }                                                             \
        if (old_##func) {                                             \
            printf("injected %s\n", #func);                           \
            return old_##func paramName;                              \
        }                                                             \
    }

injFunc(chdir, int, (path), const char *path);  // should remove the space before '('
injFunc(chmod, int, (pathname, mode), const char *pathname, mode_t mode);
injFunc(chown, int, (pathname, owner, group), const char *pathname, uid_t owner, gid_t group);
injFunc(creat, int, (pathname, mode), const char *pathname, mode_t mode);
injFunc(fopen, FILE *, (pathname, mode), const char *pathname, const char *mode);
injFunc(link, int, (oldpath, newpath), const char *oldpath, const char *newpath);
injFunc(mkdir, int, (pathname, mode), const char *pathname, mode_t mode);
injFunc(open, int, (pathname, flags), const char *pathname, int flags);
injFunc(openat, int, (dirfd, pathname, flags), int dirfd, const char *pathname, int flags);
injFunc(opendir, DIR *, (name), const char *name);
injFunc(readlink, ssize_t, (pathname, buf, bufsize), const char *pathname, char *buf, size_t bufsize);
injFunc(remove, int, (pathname), const char *pathname);
injFunc(rename, int, (oldpath, newpath), const char *oldpath, const char *newpath);
injFunc(rmdir, int, (pathname), const char *pathname);
injFunc(stat, int, (pathname, statbuf), const char *pathname, struct stat *statbuf);
injFunc(symlink, int, (target, linkpath), const char *target, const char *linkpath);
injFunc(unlink, int, (pathname), const char *pathname);
