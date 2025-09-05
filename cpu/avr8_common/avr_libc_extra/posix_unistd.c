/*
 * SPDX-FileCopyrightText: 2017 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
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

#ifndef MODULE_STDIO_NULL
void perror(const char *s)
{
    printf("%s: %s\n", s, strerror(errno));
}
#endif

/** @} */
