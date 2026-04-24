/*
 * SPDX-FileCopyrightText: 2016 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @file
 * @ingroup cpu_native
 * @brief   VFS wrappers for POSIX file I/O functions
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 */

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdint.h>
#include <sys/stat.h>
#include <unistd.h>

#include "native_internal.h"
#include "vfs.h"

/*
 * Bitmask of file descriptors opened via real_open() fallback.
 *
 * When vfs_open() returns -ENOENT because no VFS mount covers the requested
 * path (e.g. absolute paths written by the gcov runtime for .gcda files),
 * open() falls back to real_open() so that the real OS handles the I/O.
 * Subsequent read/write/close/lseek/fstat calls check this mask and route
 * to the corresponding real_* function instead of going through VFS.
 *
 * The bitmask can track at most 64 real fds (fd 0–63).  An assert in
 * _real_fd_set() catches any fd that would exceed this limit at runtime.
 * In practice the gcov runtime opens only a handful of .gcda files, so
 * this limit is not expected to be reached.
 */
#define _REAL_FD_MASK_BITS (64U)
static uint64_t _real_fd_mask;

static void _real_fd_set(int fd)
{
    assert((unsigned)fd < _REAL_FD_MASK_BITS);
    _real_fd_mask |= ((uint64_t)1 << fd);
}

static void _real_fd_clear(int fd)
{
    if ((unsigned)fd < _REAL_FD_MASK_BITS) {
        _real_fd_mask &= ~((uint64_t)1 << fd);
    }
}

static int _is_real_fd(int fd)
{
    return (unsigned)fd < _REAL_FD_MASK_BITS
           && (_real_fd_mask & ((uint64_t)1 << fd));
}

int open(const char *name, int flags, ...)
{
    unsigned mode = 0;

    if ((flags & O_CREAT)) {
        va_list ap;
        va_start(ap, flags);
        mode = va_arg(ap, unsigned);
        va_end(ap);
    }

    int fd = vfs_open(name, flags, mode);
    if (fd >= 0) {
        return fd;
    }

    /*
     * If no VFS mount covers this path, fall back to the real OS.
     * This allows third-party library code (e.g. gcov's profiling runtime)
     * to perform file I/O on paths that are not registered in RIOT's VFS.
     */
    if (fd == -ENOENT) {
        fd = real_open(name, flags, mode);
        if (fd >= 0) {
            _real_fd_set(fd);
            return fd;
        }
        return -1; /* real_open sets errno */
    }

    /* vfs returns negative error codes */
    errno = -fd;
    return -1;
}

ssize_t read(int fd, void *dest, size_t count)
{
    if (_is_real_fd(fd)) {
        return real_read(fd, dest, count);
    }

    int res = vfs_read(fd, dest, count);

    if (res < 0) {
        /* vfs returns negative error codes */
        errno = -res;
        return -1;
    }
    return res;
}

ssize_t write(int fd, const void *src, size_t count)
{
    if (_is_real_fd(fd)) {
        return real_write(fd, src, count);
    }

    int res = vfs_write(fd, src, count);

    if (res < 0) {
        /* vfs returns negative error codes */
        errno = -res;
        return -1;
    }
    return res;
}

int close(int fd)
{
    if (_is_real_fd(fd)) {
        _real_fd_clear(fd);
        return real_close(fd);
    }

    int res = vfs_close(fd);

    if (res < 0) {
        /* vfs returns negative error codes */
        errno = -res;
        return -1;
    }
    return res;
}

int fcntl(int fd, int cmd, ...)
{
    unsigned long arg;
    va_list ap;
    va_start(ap, cmd);
    arg = va_arg(ap, unsigned long);
    va_end(ap);

    int res = vfs_fcntl(fd, cmd, arg);

    if (res < 0) {
        /* vfs returns negative error codes */
        errno = -res;
        return -1;
    }
    return res;
}

off_t lseek(int fd, off_t off, int whence)
{
    if (_is_real_fd(fd)) {
        return real_lseek(fd, off, whence);
    }

    int res = vfs_lseek(fd, off, whence);

    if (res < 0) {
        /* vfs returns negative error codes */
        errno = -res;
        return -1;
    }
    return res;
}

int fstat(int fd, struct stat *buf)
{
    if (_is_real_fd(fd)) {
        return real_fstat(fd, buf);
    }

    int res = vfs_fstat(fd, buf);

    if (res < 0) {
        /* vfs returns negative error codes */
        errno = -res;
        return -1;
    }
    return 0;
}

int stat(const char *name, struct stat *st)
{
    int res = vfs_stat(name, st);

    if (res < 0) {
        /* vfs returns negative error codes */
        errno = -res;
        return -1;
    }
    return 0;
}

int unlink(const char *path)
{
    int res = vfs_unlink(path);

    if (res < 0) {
        /* vfs returns negative error codes */
        errno = -res;
        return -1;
    }
    return 0;
}
