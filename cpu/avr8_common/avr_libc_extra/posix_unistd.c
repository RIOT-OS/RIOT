/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @brief   Implements various POSIX syscalls
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include <errno.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <unistd.h>

#include "irq.h"

#ifdef MODULE_VFS
#include <fcntl.h>
#include "vfs.h"
#elif defined(MODULE_STDIO_UART)
#include "stdio_uart.h"
#endif

int open(const char *name, int flags, ...)
{
#ifdef MODULE_VFS
    unsigned mode = 0;

    if ((flags & O_CREAT)) {
        va_list ap;
        va_start(ap, flags);
        mode = va_arg(ap, unsigned);
        va_end(ap);
    }

    int fd = vfs_open(name, flags, mode);
    if (fd < 0) {
        /* vfs returns negative error codes */
        errno = -fd;
        return -1;
    }
    return fd;
#else
    (void)name;
    (void)flags;
    errno = ENODEV;
    return -1;
#endif
}

int close(int fd)
{
#ifdef MODULE_VFS
    int res = vfs_close(fd);
    if (res < 0) {
        errno = -res;
        return -1;
    }
    return res;
#else
    (void)fd;
    errno = ENODEV;
    return -1;
#endif
}

off_t lseek(int fd, off_t off, int whence)
{
#ifdef MODULE_VFS
    off_t res = vfs_lseek(fd, off, whence);
    if (res < 0) {
        errno = -res;
        return -1;
    }
    return res;
#else
    (void)fd;
    (void)off;
    (void)whence;
    errno = ENODEV;
    return -1;
#endif
}

int fcntl(int fd, int cmd, ...)
{
#ifdef MODULE_VFS
    unsigned long arg;
    va_list ap;
    va_start(ap, cmd);
    arg = va_arg(ap, unsigned long);
    va_end(ap);

    int res = vfs_fcntl(fd, cmd, arg);
    if (res < 0) {
        errno = -res;
        return -1;
    }
    return res;
#else
    (void)fd;
    (void)cmd;
    errno = ENODEV;
    return -1;
#endif
}

int fstat(int fd, struct stat *buf)
{
#ifdef MODULE_VFS
    int res = vfs_fstat(fd, buf);
    if (res < 0) {
        errno = -res;
        return -1;
    }
    return res;
#else
    (void)fd;
    (void)buf;
    errno = ENODEV;
    return -1;
#endif
}

ssize_t read(int fd, void *dest, size_t count)
{
#ifdef MODULE_VFS
    ssize_t res = vfs_read(fd, dest, count);
    if (res < 0) {
        errno = -res;
        return -1;
    }
    return res;
#elif defined(MODULE_STDIO_UART)
    if (fd == 0) {
        return stdio_read(dest, count);
    }
    errno = EBADF;
    return -1;
#else
    (void)fd;
    (void)dest;
    (void)count;
    errno = ENODEV;
    return -1;
#endif
}

ssize_t write(int fd, const void *src, size_t count)
{
#ifdef MODULE_VFS
    ssize_t res = vfs_write(fd, src, count);
    if (res < 0) {
        errno = -res;
        return -1;
    }
    return res;
#elif defined(MODULE_STDIO_UART)
    if (fd == 0) {
        return stdio_write(src, count);
    }
    errno = EBADF;
    return -1;
#else
    (void)fd;
    (void)src;
    (void)count;
    errno = ENODEV;
    return -1;
#endif
}

const char *strerror(int errnum)
{
    if (errnum < 0) {
        errnum = -errnum;
    }

    switch (errnum) {
        case 0: return "OK";
        case EDOM: return "EDOM";
        case ERANGE: return "ERANGE";
        case ENOSYS: return "ENOSYS";
        case EINTR: return "EINTR";
        case E2BIG: return "E2BIG";
        case EACCES: return "EACCES";
        case EADDRINUSE: return "EADDRINUSE";
        case EADDRNOTAVAIL: return "EADDRNOTAVAIL";
        case EAFNOSUPPORT: return "EAFNOSUPPORT";
        case EAGAIN: return "EAGAIN";
        case EALREADY: return "EALREADY";
        case EBADF: return "EBADF";
        case EBUSY: return "EBUSY";
        case ECHILD: return "ECHILD";
        case ECONNABORTED: return "ECONNABORTED";
        case ECONNREFUSED: return "ECONNREFUSED";
        case ECONNRESET: return "ECONNRESET";
        case EDEADLK: return "EDEADLK";
        case EDESTADDRREQ: return "EDESTADDRREQ";
        case EEXIST: return "EEXIST";
        case EFAULT: return "EFAULT";
        case EFBIG: return "EFBIG";
        case EHOSTUNREACH: return "EHOSTUNREACH";
        case EILSEQ: return "EILSEQ";
        case EINPROGRESS: return "EINPROGRESS";
        case EINVAL: return "EINVAL";
        case EIO: return "EIO";
        case EISCONN: return "EISCONN";
        case EISDIR: return "EISDIR";
        case ELOOP: return "ELOOP";
        case EMFILE: return "EMFILE";
        case EMLINK: return "EMLINK";
        case EMSGSIZE: return "EMSGSIZE";
        case ENAMETOOLONG: return "ENAMETOOLONG";
        case ENETDOWN: return "ENETDOWN";
        case ENETRESET: return "ENETRESET";
        case ENETUNREACH: return "ENETUNREACH";
        case ENFILE: return "ENFILE";
        case ENOBUFS: return "ENOBUFS";
        case ENODEV: return "ENODEV";
        case ENOENT: return "ENOENT";
        case ENOEXEC: return "ENOEXEC";
        case ENOLCK: return "ENOLCK";
        case ENOMEM: return "ENOMEM";
        case ENOMSG: return "ENOMSG";
        case ENOPROTOOPT: return "ENOPROTOOPT";
        case ENOSPC: return "ENOSPC";
        case ENOTCONN: return "ENOTCONN";
        case ENOTDIR: return "ENOTDIR";
        case ENOTEMPTY: return "ENOTEMPTY";
        case ENOTSOCK: return "ENOTSOCK";
        case ENOTTY: return "ENOTTY";
        case ENXIO: return "ENXIO";
        case EOPNOTSUPP: return "EOPNOTSUPP";
        case EPERM: return "EPERM";
        case EPIPE: return "EPIPE";
        case EPROTONOSUPPORT: return "EPROTONOSUPPORT";
        case EPROTOTYPE: return "EPROTOTYPE";
        case EROFS: return "EROFS";
        case ESPIPE: return "ESPIPE";
        case ESRCH: return "ESRCH";
        case ETIMEDOUT: return "ETIMEDOUT";
        case EWOULDBLOCK: return "EWOULDBLOCK";
        case EXDEV: return "EXDEV";
        default: return "unknown";
    }
}

/** @} */
