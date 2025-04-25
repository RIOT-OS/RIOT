/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *               2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    sys_newlib Newlib system call
 * @ingroup     sys
 * @brief       Newlib system call
 * @{
 *
 * @file
 * @brief       Newlib system call implementations
 *
 * @author      Michael Baar <michael.baar@fu-berlin.de>
 * @author      Stefan Pfeiffer <pfeiffer@inf.fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <errno.h>
#include <malloc.h>
#include <reent.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <sys/unistd.h>
#include <unistd.h>

#include "log.h"
#include "modules.h"
#include "periph/pm.h"
#include "sched.h"
#include "stdio_base.h"
#include "thread.h"

#if MODULE_VFS
#include "vfs.h"
#endif

#ifdef MODULE_XTIMER
#include <sys/time.h>
#include "div.h"
#include "xtimer.h"
#endif

#ifndef NUM_HEAPS
#define NUM_HEAPS 1
#endif

#ifdef __MSP430__
/* the msp430 linker scripts define the end of all memory as __stack, which in
 * turn is used as the initial stack. RIOT also uses __stack as SP on isr
 * entry.  This logic makes __stack - ISR_STACKSIZE the heap end.
 */
extern char __stack;
extern char __heap_start__;
#define _sheap __heap_start__
#define __eheap (char *)((uintptr_t)&__stack - ISR_STACKSIZE)

#else /* __MSP430__ */

/**
 * @brief manage the heap
 */
extern char _sheap;                 /* start of the heap */
extern char _eheap;                 /* end of the heap */
#define __eheap &_eheap
#endif

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
 * @brief Initialize NewLib, called by __libc_init_array() from the startup script
 */
void _init(void)
{
    /* nothing to do here */
}

/**
 * @brief Free resources on NewLib de-initialization, not used for RIOT
 */
/* __attribute__((used)) fixes linker errors when building with LTO, but without nano.specs */
__attribute__((used)) void _fini(void)
{
    /* nothing to do here */
}

/**
 * @brief Exit a program without cleaning up files
 *
 * If your system doesn't provide this, it is best to avoid linking with subroutines that
 * require it (exit, system).
 *
 * @param n     the exit code, 0 for all OK, >0 for not OK
 */
__attribute__((used)) void _exit(int n)
{
    LOG_INFO("#! exit %i: powering off\n", n);
#ifdef MODULE_PERIPH_PM
    pm_off();
#endif
    while (1) {}
}

/**
 * @brief Allocate memory from the heap.
 *
 * @return      pointer to the newly allocated memory on success
 * @return      pointer set to address `-1` on failure
 */
void *_sbrk_r(struct _reent *r, ptrdiff_t incr)
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

    if (res == (void*)UINTPTR_MAX) {
        r->_errno = ENOMEM;
    }

    irq_restore(state);
    return res;
}

/**
 * @brief Print heap statistics
 *
 * If the CPU does not provide its own heap handling and heap_stats function,
 * but instead uses the newlib_syscall_default function, this function outputs
 * the heap statistics. If the CPU provides its own heap_stats function, it
 * should define HAVE_HEAP_STATS in its cpu_conf.h file.
 */
#ifndef HAVE_HEAP_STATS
__attribute__((weak)) void heap_stats(void)
{
    struct mallinfo minfo = mallinfo();
    long int heap_size = 0;

    for (unsigned int i = 0; i < NUM_HEAPS; i++) {
        heap_size += heaps[i].end - heaps[i].start;
    }

    printf("heap: %ld (used %d, free %ld) [bytes]\n",
           heap_size, minfo.uordblks, heap_size - minfo.uordblks);
}
#endif /* HAVE_HEAP_STATS */

/**
 * @brief Get the process-ID of the current thread
 *
 * @return      the process ID of the current thread
 */
pid_t _getpid(void)
{
    return thread_getpid();
}

/**
 * @brief Get the process-ID of the current thread
 *
 * @return      the process ID of the current thread
 */
pid_t _getpid_r(struct _reent *ptr)
{
    (void) ptr;
    return thread_getpid();
}

/**
 * @brief Send a signal to a given thread
 *
 * @param r     TODO
 * @param pid   TODO
 * @param sig   TODO
 *
 * @return      TODO
 */
__attribute__ ((weak))
int _kill_r(struct _reent *r, pid_t pid, int sig)
{
    (void) pid;
    (void) sig;
    r->_errno = ESRCH;                      /* not implemented yet */
    return -1;
}

#if MODULE_VFS
/**
 * @brief Open a file
 *
 * This is a wrapper around @c vfs_open
 *
 * @param r     pointer to reent structure
 * @param name  file name to open
 * @param flags flags, see man 3p open
 * @param mode  mode, file creation mode if the file is created when opening
 *
 * @return      fd number (>= 0) on success
 * @return      -1 on error, @c r->_errno set to a constant from errno.h to indicate the error
 */
int _open_r(struct _reent *r, const char *name, int flags, int mode)
{
    int fd = vfs_open(name, flags, mode);
    if (fd < 0) {
        /* vfs returns negative error codes */
        r->_errno = -fd;
        return -1;
    }
    return fd;
}

/**
 * @brief Read bytes from an open file
 *
 * This is a wrapper around @c vfs_read
 *
 * @param[in]  r      pointer to reent structure
 * @param[in]  fd     open file descriptor obtained from @c open()
 * @param[out] dest   destination buffer
 * @param[in]  count  maximum number of bytes to read
 *
 * @return       number of bytes read on success
 * @return       -1 on error, @c r->_errno set to a constant from errno.h to indicate the error
 */
_ssize_t _read_r(struct _reent *r, int fd, void *dest, size_t count)
{
    int res = vfs_read(fd, dest, count);
    if (res < 0) {
        /* vfs returns negative error codes */
        r->_errno = -res;
        return -1;
    }
    return res;
}

/**
 * @brief Write bytes to an open file
 *
 * This is a wrapper around @c vfs_write
 *
 * @param[in]  r      pointer to reent structure
 * @param[in]  fd     open file descriptor obtained from @c open()
 * @param[in]  src    source data buffer
 * @param[in]  count  maximum number of bytes to write
 *
 * @return       number of bytes written on success
 * @return       -1 on error, @c r->_errno set to a constant from errno.h to indicate the error
 */
_ssize_t _write_r(struct _reent *r, int fd, const void *src, size_t count)
{
    int res = vfs_write(fd, src, count);
    if (res < 0) {
        /* vfs returns negative error codes */
        r->_errno = -res;
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
 * @param[in]  r      pointer to reent structure
 * @param[in]  fd     open file descriptor obtained from @c open()
 *
 * @return       0 on success
 * @return       -1 on error, @c r->_errno set to a constant from errno.h to indicate the error
 */
int _close_r(struct _reent *r, int fd)
{
    int res = vfs_close(fd);
    if (res < 0) {
        /* vfs returns negative error codes */
        r->_errno = -res;
        return -1;
    }
    return res;
}

/**
 * @brief Query or set options on an open file
 *
 * This is a wrapper around @c vfs_fcntl
 *
 * @param[in]  r      pointer to reent structure
 * @param[in]  fd     open file descriptor obtained from @c open()
 * @param[in]  cmd    fcntl command, see man 3p fcntl
 * @param[in]  arg    argument to fcntl command, see man 3p fcntl
 *
 * @return       0 on success
 * @return       -1 on error, @c r->_errno set to a constant from errno.h to indicate the error
 */
int _fcntl_r (struct _reent *r, int fd, int cmd, int arg)
{
    int res = vfs_fcntl(fd, cmd, arg);
    if (res < 0) {
        /* vfs returns negative error codes */
        r->_errno = -res;
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
 * @param[in]  r        pointer to reent structure
 * @param[in]  fd       open file descriptor obtained from @c open()
 * @param[in]  off      seek offset
 * @param[in]  whence   determines the seek method, see detailed description
 *
 * @return the new seek location in the file on success
 * @return -1 on error, @c r->_errno set to a constant from errno.h to indicate the error
 */
_off_t _lseek_r(struct _reent *r, int fd, _off_t off, int whence)
{
    int res = vfs_lseek(fd, off, whence);
    if (res < 0) {
        /* vfs returns negative error codes */
        r->_errno = -res;
        return -1;
    }
    return res;
}

/**
 * @brief Get status of an open file
 *
 * This is a wrapper around @c vfs_fstat
 *
 * @param[in]  r        pointer to reent structure
 * @param[in]  fd       open file descriptor obtained from @c open()
 * @param[out] buf      pointer to stat struct to fill
 *
 * @return 0 on success
 * @return -1 on error, @c r->_errno set to a constant from errno.h to indicate the error
 */
int _fstat_r(struct _reent *r, int fd, struct stat *buf)
{
    int res = vfs_fstat(fd, buf);
    if (res < 0) {
        /* vfs returns negative error codes */
        r->_errno = -res;
        return -1;
    }
    return 0;
}

/**
 * @brief Status of a file (by name)
 *
 * This is a wrapper around @c vfs_fstat
 *
 * @param[in]  r        pointer to reent structure
 * @param[in]  name     path to file
 * @param[out] st       pointer to stat struct to fill
 *
 * @return 0 on success
 * @return -1 on error, @c r->_errno set to a constant from errno.h to indicate the error
 */
int _stat_r(struct _reent *r, const char *name, struct stat *st)
{
    int res = vfs_stat(name, st);
    if (res < 0) {
        /* vfs returns negative error codes */
        r->_errno = -res;
        return -1;
    }
    return 0;
}

/**
 * @brief  Unlink (delete) a file
 *
 * @param[in]  r        pointer to reent structure
 * @param[in]  path     path to file to be deleted
 *
 * @return 0 on success
 * @return -1 on error, @c r->_errno set to a constant from errno.h to indicate the error
 */
int _unlink_r(struct _reent *r, const char *path)
{
    int res = vfs_unlink(path);
    if (res < 0) {
        /* vfs returns negative error codes */
        r->_errno = -res;
        return -1;
    }
    return 0;
}

#else /* MODULE_VFS */

/* Fallback stdio_uart wrappers for when VFS is not used, does not allow any
 * other file access */
/*
 * Fallback read function
 *
 * All input is read from stdio_uart regardless of fd number. The function will
 * block until a byte is actually read.
 *
 * Note: the read function does not buffer - data will be lost if the function is not
 * called fast enough.
 */
_ssize_t _read_r(struct _reent *r, int fd, void *buffer, size_t count)
{
    (void)r;
    (void)fd;
    return stdio_read(buffer, count);
}

/*
 * Fallback write function
 *
 * All output is directed to stdio_uart, independent of the given file descriptor.
 * The write call will further block until the byte is actually written to the UART.
 */
_ssize_t _write_r(struct _reent *r, int fd, const void *data, size_t count)
{
    (void) r;
    (void) fd;
    return stdio_write(data, count);
}

/* Stubs to avoid linking errors, these functions do not have any effect */
int _open_r(struct _reent *r, const char *name, int flags, int mode)
{
    (void) name;
    (void) flags;
    (void) mode;
    r->_errno = ENODEV;
    return -1;
}

int _close_r(struct _reent *r, int fd)
{
    (void) fd;
    r->_errno = ENODEV;
    return -1;
}

_off_t _lseek_r(struct _reent *r, int fd, _off_t pos, int dir)
{
    (void) fd;
    (void) pos;
    (void) dir;
    r->_errno = ENODEV;
    return -1;
}

int _fstat_r(struct _reent *r, int fd, struct stat *st)
{
    (void) fd;
    (void) st;
    r->_errno = ENODEV;
    return -1;
}

int _stat_r(struct _reent *r, const char *name, struct stat *st)
{
    (void) name;
    (void) st;
    r->_errno = ENODEV;
    return -1;
}

int _unlink_r(struct _reent *r, const char *path)
{
    (void) path;
    r->_errno = ENODEV;
    return -1;
}
#endif /* MODULE_VFS */

/**
 * Create a hard link (not implemented).
 *
 * @todo    Not implemented.
 *
 * @return  -1. Sets errno to ENOSYS.
 */
int _link_r(struct _reent *ptr, const char *old_name, const char *new_name)
{
    (void)old_name;
    (void)new_name;

    ptr->_errno = ENOSYS;

    return -1;
}

/**
 * @brief Query whether output stream is a terminal
 *
 * @param r     TODO
 * @param fd    TODO
 *
 * @return      TODO
 */
int _isatty_r(struct _reent *r, int fd)
{
    r->_errno = 0;

    if (fd == STDIN_FILENO || fd == STDOUT_FILENO || fd == STDERR_FILENO) {
        return 1;
    }

    return 0;
}

/**
 * @brief Send a signal to a thread
 *
 * @param[in] pid the pid to send to
 * @param[in] sig the signal to send
 *
 * @return TODO
 */
__attribute__ ((weak))
int _kill(pid_t pid, int sig)
{
    (void) pid;
    (void) sig;
    errno = ESRCH;                         /* not implemented yet */
    return -1;
}

#if (IS_USED(MODULE_LIBC_GETTIMEOFDAY))
int _gettimeofday_r(struct _reent *r, struct timeval *restrict tp, void *restrict tzp)
{
    (void)tzp;
    (void)r;
    uint64_t now = xtimer_now_usec64();
    tp->tv_sec = div_u64_by_1000000(now);
    tp->tv_usec = now - (tp->tv_sec * US_PER_SEC);
    return 0;
}
#endif

/**
 * Current process times (not implemented).
 *
 * @param[out]  ptms    Not modified.
 *
 * @return  -1, this function always fails. errno is set to ENOSYS.
 */
clock_t _times_r(struct _reent *ptr, struct tms *ptms)
{
    (void)ptms;
    ptr->_errno = ENOSYS;

    return (-1);
}
