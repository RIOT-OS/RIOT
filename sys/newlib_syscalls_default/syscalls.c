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

#include <unistd.h>
#include <reent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/unistd.h>
#include <stdint.h>

#include "cpu.h"
#include "board.h"
#include "sched.h"
#include "thread.h"
#include "irq.h"
#include "log.h"
#include "periph/pm.h"

#include "stdio_base.h"

/**
 * @brief manage the heap
 */
extern char _sheap;                 /* start of the heap */
extern char _eheap;                 /* end of the heap */
char *heap_top = &_sheap + 4;

/* MIPS newlib crt implements _init,_fini and _exit and manages the heap */
#ifndef __mips__
/**
 * @brief Initialize NewLib, called by __libc_init_array() from the startup script
 */
void _init(void)
{
    /* FIXME: this logic should be moved to newlib_syscalls_riot_serial_rw,
     *        newlib_syscalls_riot_serial_vfs, or even better, a separate submodule*/
#if !(MODULE_NEWLIB_STUBS_RW)
    stdio_init();
#endif
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
void _exit(int n)
{
    LOG_INFO("#! exit %i: powering off\n", n);
    pm_off();
    while(1);
}

/**
 * @brief Allocate memory from the heap.
 *
 * The current heap implementation is very rudimentary, it is only able to allocate
 * memory. But it does not have any means to free memory again
 *
 * @return      pointer to the newly allocated memory on success
 * @return      pointer set to address `-1` on failure
 */
void *_sbrk_r(struct _reent *r, ptrdiff_t incr)
{
    unsigned int state = irq_disable();
    void *res = heap_top;

    if ((heap_top + incr > &_eheap) || (heap_top + incr < &_sheap)) {
        r->_errno = ENOMEM;
        res = (void *)-1;
    }
    else {
        heap_top += incr;
    }

    irq_restore(state);
    return res;
}

#endif /*__mips__*/

/**
 * @brief Get the process-ID of the current thread
 *
 * @return      the process ID of the current thread
 */
pid_t _getpid(void)
{
    return sched_active_pid;
}

/**
 * @brief Get the process-ID of the current thread
 *
 * @return      the process ID of the current thread
 */
pid_t _getpid_r(struct _reent *ptr)
{
    (void) ptr;
    return sched_active_pid;
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

    if(fd == STDIN_FILENO || fd == STDOUT_FILENO || fd == STDERR_FILENO) {
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
