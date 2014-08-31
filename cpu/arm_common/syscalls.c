/*
 * syscalls.c - arm system calls
 * Copyright (C) 2013 Oliver Hahm <oliver.hahm@inria.fr>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @file
 * @ingroup     arm_common
 * @brief       LPC2387 NewLib system calls implementation
 *
 * @author      Michael Baar <michael.baar@fu-berlin.de>
 *
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/unistd.h>
#include <stdint.h>
#include <sys/time.h>

#include "arm_cpu.h"
// core
#include "kernel.h"
#include "irq.h"
#include "io.h"
#if defined MODULE_RTC
#include "rtc.h"
#elif defined MODULE_VTIMER
#include "vtimer.h"
#endif

/* When using the HAL standard in and out are handled by HAL
   devices. */
#if FEUERWARE_CONF_ENABLE_HAL
#include "hal.h"
#include "interface-chardevice.h"
#include "hal-syscalls.h"
#endif

#define DEBUG_SYSCALLS          0
#if DEBUG_SYSCALLS
#define PRINTF(...)             printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

#ifdef MODULE_FAT
#include "ff_ansi.h"
#endif

/**
 * @name Heaps (defined in linker script)
 * @{
 */

extern uintptr_t __heap1_start;     ///< start of heap memory space
extern uintptr_t __heap1_max;       ///< maximum for end of heap memory space
extern uintptr_t __heap2_start;     ///< start of heap memory space
extern uintptr_t __heap2_max;       ///< maximum for end of heap memory space
extern uintptr_t __heap3_start;     ///< start of heap memory space
extern uintptr_t __heap3_max;       ///< maximum for end of heap memory space

/*-----------------------------------------------------------------------------------*/
void __assert_func(const char *file, int line, const char *func, const char *failedexpr)
{
    printf("#!assertion %s failed\n\t%s() in %s:%d\n", failedexpr, func, file, line);
    _exit(3);
}
/*-----------------------------------------------------------------------------------*/
void __assert(const char *file, int line, const char *failedexpr)
{
    __assert_func(file, line, "?", failedexpr);
}

/*---------------------------------------------------------------------------*/
int _isatty_r(struct _reent *r, int fd)
{
    r->_errno = 0;

    if (fd == STDOUT_FILENO || fd == STDERR_FILENO) {
        return 1;
    }
    else {
        return 0;
    }
}
/*---------------------------------------------------------------------------*/
_off_t _lseek_r(struct _reent *r, int fd, _off_t pos, int whence)
{
    /* to get rid of gcc warnings */
    (void) fd;
    (void) pos;
    (void) whence;
    _off_t result = -1;
    PRINTF("lseek [%i] pos %li whence %i\n", fd, pos, whence);

    r->_errno = ENODEV;
#ifdef MODULE_FAT
    result = ff_lseek_r(r, fd, pos, whence);
#endif

    PRINTF("lseek returned %li (0 is success)\n", result);
    return result;
}
/*---------------------------------------------------------------------------*/
int _open_r(struct _reent *r, const char *name, int mode)
{
    /* to get rid of gcc warnings */
    (void) name;
    (void) mode;
    int ret = -1;
    PRINTF("open '%s' mode %#x\n", name, mode);

    r->_errno = ENODEV; // no such device
#ifdef MODULE_FAT
    ret = ff_open_r(r, name, mode);
#endif

    PRINTF("open [%i] errno %i\n", ret, r->_errno);
    return ret;
}
/*---------------------------------------------------------------------------*/
int _stat_r(struct _reent *r, char *name, struct stat *st)
{
    /* to get rid of gcc warnings */
    (void) name;
    (void) st;
    int ret = -1;
    PRINTF("_stat_r '%s' \n", name);
    r->_errno = ENODEV; // no such device
#ifdef MODULE_FAT
    ret = ff_stat_r(r, name, st);
#endif
    PRINTF("_stat_r [%i] errno %i\n", ret, r->_errno);
    return ret;
}
/*---------------------------------------------------------------------------*/
int _fstat_r(struct _reent *r, int fd, struct stat *st)
{
    int ret = -1;

    r->_errno = 0;
    memset(st, 0, sizeof(*st));

    if (fd == STDOUT_FILENO || fd == STDERR_FILENO) {
        st->st_mode = S_IFCHR;
        ret = 0;
    }
    else {

#ifdef MODULE_FAT
        PRINTF("_fstat_r '%i' \n", fd);

        ret = ff_fstat_r(r, fd, st);
        PRINTF("_fstat_r [%i] errno %i\n", ret, r->_errno);

#else
        r->_errno = ENODEV;
#endif
    }

    return ret;
}
/*---------------------------------------------------------------------------*/
int _write_r(struct _reent *r, int fd, const void *data, unsigned int count)
{
    int result = EOF;
    r->_errno = EBADF;

    switch(fd) {
        case STDOUT_FILENO:
        case STDERR_FILENO:
#if FEUERWARE_CONF_ENABLE_HAL
            if (stdio != NULL) {
                result = chardevice_write(stdio, (char *)data, count);
            }
            else if (hal_state == HAL_NOT_INITIALIZED) {
                result = fw_puts((char *)data, count);
            }

#else
            result = fw_puts((char *)data, count);
#endif
            break;

        default:
#ifdef MODULE_FAT
            result = ff_write_r(r, fd, data, count);
#endif
            PRINTF("write [%i] data @%p count %i\n", fd, data, count);

            PRINTF("write [%i] returned %i errno %i\n", fd, result, r->_errno);
            break;
    }

    return result;
}
/*---------------------------------------------------------------------------*/
int _read_r(struct _reent *r, int fd, void *buffer, unsigned int count)
{
    /* to get rid of gcc warnings */
    (void) fd;
    (void) buffer;
    (void) count;
    int result = -1;
    r->_errno = EBADF;
#ifdef MODULE_FAT
    result = ff_read_r(r, fd, buffer, count);
#endif
    PRINTF("read [%i] buffer @%p count %i\n", fd, buffer, count);
    PRINTF("read [%i] returned %i\n", fd, result);

    return result;
}
/*---------------------------------------------------------------------------*/
int _close_r(struct _reent *r, int fd)
{
    (void) fd;
    int result = -1;
    r->_errno = EBADF;
#ifdef MODULE_FAT
    ret = ff_close_r(r, fd);
#endif
    PRINTF("close [%i]\n", fd);
    PRINTF("close returned %i errno %i\n", result, errno);

    return result;
}
/*---------------------------------------------------------------------------*/
int _unlink_r(struct _reent *r, char *path)
{
    /* get rid of gcc warnings */
    (void) path;

    int result = -1;
    r->_errno = ENODEV;
#ifdef MODULE_FAT
    result = ff_unlink_r(r, path);
#endif
    PRINTF("unlink '%s'\n", path);
    PRINTF("unlink returned %i errno %i\n", result, errno);

    return result;
}
/*---------------------------------------------------------------------------*/
void _exit(int n)
{
    printf("#!exit %i: resetting\n", n);

    stdio_flush();
    arm_reset();

    while (1);
}
/*---------------------------------------------------------------------------*/
pid_t _getpid(void)
{
    return (pid_t) sched_active_pid;
}
/*---------------------------------------------------------------------------*/
int _kill_r(struct _reent *r, int pid, int sig)
{
    (void) pid;
    (void) sig;
    /* not implemented */
    r->_errno = ESRCH;      // no such process
    return -1;
}

void _init(void) {}
void _fini(void) {}
