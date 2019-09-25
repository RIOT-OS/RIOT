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

#include "log.h"
#include "periph/pm.h"
#include "stdio_base.h"

uint8_t sbrk[1];

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
    (void) pid;
    (void) sig;
    errno = ESRCH;                         /* not implemented yet */
    return -1;
}

/* File descriptor pointers */
int picolibc_get(FILE *file)
{
    (void)file;
    char c = 0;
    stdio_read(&c, 1);
    return c;
}

int picolibc_put(char c, FILE *file)
{
    (void)file;
    stdio_write(&c, 1);
    return 1;
}
FILE picolibc_stdio =
    FDEV_SETUP_STREAM(picolibc_put, picolibc_get, NULL, _FDEV_SETUP_RW);

FILE *const __iob[] = {
    &picolibc_stdio,    /* stdin  */
    &picolibc_stdio,    /* stdout */
    &picolibc_stdio,    /* stderr */
};

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
    int fd = vfs_open(name, flags, mode);
    if (fd < 0) {
        /* vfs returns negative error codes */
        errno = -fd;
        return -1;
    }
    return fd;
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
    int res = vfs_read(fd, dest, count);
    if (res < 0) {
        /* vfs returns negative error codes */
        errno = -res;
        return -1;
    }
    return res;
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
    int res = vfs_write(fd, src, count);
    if (res < 0) {
        /* vfs returns negative error codes */
        errno = -res;
        return -1;
    }
    return res;
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
    int res = vfs_close(fd);
    if (res < 0) {
        /* vfs returns negative error codes */
        errno = -res;
        return -1;
    }
    return res;
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
int fcntl (int fd, int cmd, int arg)
{
    int res = vfs_fcntl(fd, cmd, arg);
    if (res < 0) {
        /* vfs returns negative error codes */
        errno = -res;
        return -1;
    }
    return res;
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
    int res = vfs_lseek(fd, off, whence);
    if (res < 0) {
        /* vfs returns negative error codes */
        errno = -res;
        return -1;
    }
    return res;
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
    int res = vfs_fstat(fd, buf);
    if (res < 0) {
        /* vfs returns negative error codes */
        errno = -res;
        return -1;
    }
    return 0;
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
    int res = vfs_stat(name, st);
    if (res < 0) {
        /* vfs returns negative error codes */
        errno = -res;
        return -1;
    }
    return 0;
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
    int res = vfs_unlink(path);
    if (res < 0) {
        /* vfs returns negative error codes */
        errno = -res;
        return -1;
    }
    return 0;
}

#endif /* MODULE_VFS */
