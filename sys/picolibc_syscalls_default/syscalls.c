/*
 * Copyright (C) 2019 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    sys_picolibc PicoLibc system call
 * @ingroup     sys
 * @brief       PicoLibc system call
 * @{
 *
 * @file
 * @brief       PicoLibc system call implementations
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#include <errno.h>
#include <stdio.h>
#include <sys/times.h>
#include <unistd.h>

#include "irq.h"
#include "log.h"
#include "periph/pm.h"
#include "stdio_base.h"

#ifndef NUM_HEAPS
#define NUM_HEAPS 1
#endif

/**
 * @brief manage the heap
 */
extern char _sheap;                 /* start of the heap */
extern char _eheap;                 /* end of the heap */
#define __eheap &_eheap

/**
 * @brief Additional heap sections that may be defined in the linkerscript.
 *
 *        The compiler should not generate references to those symbols if
 *        they are not used, so only provide them if additional memory sections
 *        that can be used as heap are available.
 * @{
 */
extern char _sheap1;
extern char _eheap1;

extern char _sheap2;
extern char _eheap2;

extern char _sheap3;
extern char _eheap3;
/* @} */

struct heap {
    char* start;
    char* end;
};

static char *heap_top[NUM_HEAPS] = {
    &_sheap,
#if NUM_HEAPS > 1
    &_sheap1,
#endif
#if NUM_HEAPS > 2
    &_sheap2,
#endif
#if NUM_HEAPS > 3
    &_sheap3,
#endif
#if NUM_HEAPS > 4
#error "Unsupported NUM_HEAPS value, edit newlib_syscalls_default/syscalls.c to add more heaps."
#endif
};

static const struct heap heaps[NUM_HEAPS] = {
    {
        .start = &_sheap,
        .end   = __eheap
    },
#if NUM_HEAPS > 1
    {
        .start = &_sheap1,
        .end   = &_eheap1
    },
#endif
#if NUM_HEAPS > 2
    {
        .start = &_sheap2,
        .end   = &_eheap2
    },
#endif
#if NUM_HEAPS > 3
    {
        .start = &_sheap3,
        .end   = &_eheap3
    },
#endif
};

/**
 * @brief Exit a program without cleaning up files
 *
 * If your system doesn't provide this, it is best to avoid linking with subroutines that
 * require it (exit, system).
 *
 * @param n     the exit code, 0 for all OK, >0 for not OK
 */
void __attribute__((__noreturn__))
_exit(int n)
{
    LOG_INFO("#! exit %i: powering off\n", n);
    pm_off();
    while(1);
}

/**
 * @brief Allocate memory from the heap.
 *
 * @return      pointer to the newly allocated memory on success
 * @return      pointer set to address `-1` on failure
 */
void *sbrk(ptrdiff_t incr)
{
    void *res = (void*)UINTPTR_MAX;
    unsigned int state = irq_disable();

    for (unsigned i = 0; i < NUM_HEAPS; ++i) {
        if ((heap_top[i] + incr > heaps[i].end) ||
            (heap_top[i] + incr < heaps[i].start)) {
            continue;
        }

        res = heap_top[i];
        heap_top[i] += incr;
        break;
    }

    irq_restore(state);
    return res;
}

/**
 * @brief Send a signal to a thread
 *
 * @param[in] pid the pid to send to
 * @param[in] sig the signal to send
 *
 * @return    always returns -1 to signal error
 */
__attribute__ ((weak))
int kill(pid_t pid, int sig)
{
    (void)pid;
    (void)sig;
    errno = ESRCH;                         /* not implemented yet */
    return -1;
}

#include "mutex.h"

#ifndef PICOLIBC_STDOUT_BUFSIZE
#define PICOLIBC_STDOUT_BUFSIZE 64
#endif

#ifdef MODULE_PICOLIBC_STDOUT_BUFFERED
static mutex_t picolibc_put_mutex = MUTEX_INIT;
static char picolibc_stdout[PICOLIBC_STDOUT_BUFSIZE];
static int picolibc_stdout_queued;

static void _picolibc_flush(void)
{
    if (picolibc_stdout_queued) {
        stdio_write(picolibc_stdout, picolibc_stdout_queued);
        picolibc_stdout_queued = 0;
    }
}

static int picolibc_put(char c, FILE *file)
{
    (void)file;

    mutex_lock(&picolibc_put_mutex);
    picolibc_stdout[picolibc_stdout_queued++] = c;

    if (picolibc_stdout_queued == PICOLIBC_STDOUT_BUFSIZE || c == '\n') {
        _picolibc_flush();
    }

    mutex_unlock(&picolibc_put_mutex);
    return 1;
}

static int picolibc_flush(FILE *file)
{
    (void)file;
    mutex_lock(&picolibc_put_mutex);
    _picolibc_flush();
    mutex_unlock(&picolibc_put_mutex);
    return 0;
}

#else
int picolibc_put(char c, FILE *file)
{
    (void)file;
    stdio_write(&c, 1);
    return 1;
}

static int picolibc_flush(FILE *file)
{
    (void)file;
    return 0;
}

#endif

static int picolibc_get(FILE *file)
{
    (void)file;
    picolibc_flush(NULL);
    char c = 0;
    stdio_read(&c, 1);
    return c;
}

FILE picolibc_stdio =
    FDEV_SETUP_STREAM(picolibc_put, picolibc_get, picolibc_flush, _FDEV_SETUP_RW);

#ifdef PICOLIBC_STDIO_GLOBALS
#ifdef __strong_reference
/* This saves two const pointers.
 * See https://github.com/RIOT-OS/RIOT/pull/17001#issuecomment-945936918
 */
#define STDIO_ALIAS(x) __strong_reference(stdin, x);
#else
#define STDIO_ALIAS(x) FILE *const x = &__picolibc_stdio;
#endif

FILE *const stdin = &picolibc_stdio;
STDIO_ALIAS(stdout);
STDIO_ALIAS(stderr);
#else
FILE *const __iob[] = {
    &picolibc_stdio,        /* stdin  */
    &picolibc_stdio,        /* stdout */
    &picolibc_stdio,        /* stderr */
};
#endif

#include <thread.h>
/**
 * @brief Get the process-ID of the current thread
 *
 * @return      the process ID of the current thread
 */
pid_t getpid(void)
{
    return thread_getpid();
}

#if MODULE_VFS
#include "vfs.h"
#else
#include <sys/stat.h>
#endif

/**
 * @brief Open a file
 *
 * This is a wrapper around @c vfs_open
 *
 * @param name  file name to open
 * @param flags flags, see man 3p open
 * @param mode  mode, file creation mode if the file is created when opening
 *
 * @return      fd number (>= 0) on success
 * @return      -1 on error, @c errno set to a constant from errno.h to indicate the error
 */
int open(const char *name, int flags, int mode)
{
#ifdef MODULE_VFS
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
    (void)mode;
    errno = ENODEV;
    return -1;
#endif
}

/**
 * @brief Read bytes from an open file
 *
 * This is a wrapper around @c vfs_read
 *
 * @param[in]  fd     open file descriptor obtained from @c open()
 * @param[out] dest   destination buffer
 * @param[in]  count  maximum number of bytes to read
 *
 * @return       number of bytes read on success
 * @return       -1 on error, @c errno set to a constant from errno.h to indicate the error
 */
_READ_WRITE_RETURN_TYPE read(int fd, void *dest, size_t count)
{
#ifdef MODULE_VFS
    int res = vfs_read(fd, dest, count);
    if (res < 0) {
        /* vfs returns negative error codes */
        errno = -res;
        return -1;
    }
    return res;
#else
    if (fd != STDIN_FILENO) {
        errno = ENOTSUP;
        return -1;
    }

    return stdio_read(dest, count);
#endif
}

/**
 * @brief Write bytes to an open file
 *
 * This is a wrapper around @c vfs_write
 *
 * @param[in]  fd     open file descriptor obtained from @c open()
 * @param[in]  src    source data buffer
 * @param[in]  count  maximum number of bytes to write
 *
 * @return       number of bytes written on success
 * @return       -1 on error, @c errno set to a constant from errno.h to indicate the error
 */
_READ_WRITE_RETURN_TYPE write(int fd, const void *src, size_t count)
{
#ifdef MODULE_VFS
    int res = vfs_write(fd, src, count);
    if (res < 0) {
        /* vfs returns negative error codes */
        errno = -res;
        return -1;
    }
    return res;
#else
    if (fd != STDOUT_FILENO && fd != STDERR_FILENO) {
        errno = ENOTSUP;
        return -1;
    }

    return stdio_write(src, count);
#endif
}

/**
 * @brief Close an open file
 *
 * This is a wrapper around @c vfs_close
 *
 * If this call returns an error, the fd should still be considered invalid and
 * no further attempt to use it shall be made, not even to retry @c close()
 *
 * @param[in]  fd     open file descriptor obtained from @c open()
 *
 * @return       0 on success
 * @return       -1 on error, @c errno set to a constant from errno.h to indicate the error
 */
int close(int fd)
{
#ifdef MODULE_VFS
    int res = vfs_close(fd);
    if (res < 0) {
        /* vfs returns negative error codes */
        errno = -res;
        return -1;
    }
    return res;
#else
    (void)fd;
    errno = ENOTSUP;
    return -1;
#endif
}

/**
 * Current process times (not implemented).
 *
 * @param[out]  ptms    Not modified.
 *
 * @return  -1, this function always fails. errno is set to ENOSYS.
 */
clock_t times(struct tms *ptms)
{
    (void)ptms;
    errno = ENOSYS;

    return (-1);
}

/**
 * @brief Query or set options on an open file
 *
 * This is a wrapper around @c vfs_fcntl
 *
 * @param[in]  fd     open file descriptor obtained from @c open()
 * @param[in]  cmd    fcntl command, see man 3p fcntl
 * @param[in]  arg    argument to fcntl command, see man 3p fcntl
 *
 * @return       0 on success
 * @return       -1 on error, @c errno set to a constant from errno.h to indicate the error
 */
int fcntl(int fd, int cmd, int arg)
{
#ifdef MODULE_VFS
    int res = vfs_fcntl(fd, cmd, arg);
    if (res < 0) {
        /* vfs returns negative error codes */
        errno = -res;
        return -1;
    }
    return res;
#else
    (void)fd;
    (void)cmd;
    (void)arg;
    errno = ENOTSUP;
    return -1;
#endif
}

/**
 * @brief Seek to position in file
 *
 * This is a wrapper around @c vfs_lseek
 *
 * @p whence determines the function of the seek and should be set to one of
 * the following values:
 *
 *  - @c SEEK_SET: Seek to absolute offset @p off
 *  - @c SEEK_CUR: Seek to current location + @p off
 *  - @c SEEK_END: Seek to end of file + @p off
 *
 * @param[in]  fd       open file descriptor obtained from @c open()
 * @param[in]  off      seek offset
 * @param[in]  whence   determines the seek method, see detailed description
 *
 * @return the new seek location in the file on success
 * @return -1 on error, @c errno set to a constant from errno.h to indicate the error
 */
off_t lseek(int fd, _off_t off, int whence)
{
#ifdef MODULE_VFS
    int res = vfs_lseek(fd, off, whence);
    if (res < 0) {
        /* vfs returns negative error codes */
        errno = -res;
        return -1;
    }
    return res;
#else
    (void)fd;
    (void)off;
    (void)whence;
    errno = ENOTSUP;
    return -1;
#endif
}

/**
 * @brief Sets the file position indicator to the the beginning of the file.
 *
 * @param[in]  stream   open file descriptor obtained from @c fopen()
 */
void rewind(FILE *stream)
{
    fseek(stream, 0L, SEEK_SET);
}

/**
 * @brief Get status of an open file
 *
 * This is a wrapper around @c vfs_fstat
 *
 * @param[in]  fd       open file descriptor obtained from @c open()
 * @param[out] buf      pointer to stat struct to fill
 *
 * @return 0 on success
 * @return -1 on error, @c errno set to a constant from errno.h to indicate the error
 */
int fstat(int fd, struct stat *buf)
{
#ifdef MODULE_VFS
    int res = vfs_fstat(fd, buf);
    if (res < 0) {
        /* vfs returns negative error codes */
        errno = -res;
        return -1;
    }
    return 0;
#else
    (void)fd;
    (void)buf;
    errno = ENOTSUP;
    return -1;
#endif
}

/**
 * @brief Status of a file (by name)
 *
 * This is a wrapper around @c vfs_fstat
 *
 * @param[in]  name     path to file
 * @param[out] buf      pointer to stat struct to fill
 *
 * @return 0 on success
 * @return -1 on error, @c errno set to a constant from errno.h to indicate the error
 */
int stat(const char *name, struct stat *st)
{
#ifdef MODULE_VFS
    int res = vfs_stat(name, st);
    if (res < 0) {
        /* vfs returns negative error codes */
        errno = -res;
        return -1;
    }
    return 0;
#else
    (void)name;
    (void)st;
    errno = ENODEV;
    return -1;
#endif
}

/**
 * @brief  Unlink (delete) a file
 *
 * @param[in]  path     path to file to be deleted
 *
 * @return 0 on success
 * @return -1 on error, @c errno set to a constant from errno.h to indicate the error
 */
int unlink(const char *path)
{
#ifdef MODULE_VFS
    int res = vfs_unlink(path);
    if (res < 0) {
        /* vfs returns negative error codes */
        errno = -res;
        return -1;
    }
    return 0;
#else
    (void)path;
    errno = ENODEV;
    return -1;
#endif
}

/**
 * @brief  Deletes a directory, which must be empty
 *
 * @param[in]  path     path to directory to be deleted
 *
 * @return 0 on success
 * @return -1 on error, @c errno set to a constant from errno.h to indicate the error
 */
int rmdir(const char *path)
{
#ifdef MODULE_VFS
    int res = vfs_rmdir(path);
    if (res < 0) {
        /* vfs returns negative error codes */
        errno = -res;
        return -1;
    }
    return 0;
#else
    (void)path;
    errno = ENODEV;
    return -1;
#endif
}

/**
 * @brief  Remove (delete) a file or directory
 *
 * remove()  deletes  a name from the filesystem.
 * It calls unlink(2) for files, and rmdir(2) for directories.
 *
 * @param[in]  path     path to file or directory to be deleted
 *
 * @return 0 on success
 * @return -1 on error, @c errno set to a constant from errno.h to indicate the error
 */
int remove(const char *path)
{
    if (unlink(path) == 0) {
        return 0;
    }

    return rmdir(path);
}
