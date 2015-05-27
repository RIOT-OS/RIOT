/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *               2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     sys_newlib
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/unistd.h>
#include <stdint.h>

#include "cpu.h"
#include "board.h"
#include "thread.h"
#include "kernel.h"
#include "mutex.h"
#include "ringbuffer.h"
#include "irq.h"
#include "periph/uart.h"

#ifdef MODULE_UART0
#include "board_uart0.h"
#endif

/**
 * @brief manage the heap
 */
extern char _sheap;                 /* start of the heap */
extern char _eheap;                 /* end of the heap */
caddr_t heap_top = (caddr_t)&_sheap + 4;

#ifndef MODULE_UART0
/**
 * @brief use mutex for waiting on incoming UART chars
 */
static mutex_t uart_rx_mutex;
static char rx_buf_mem[STDIO_RX_BUFSIZE];
static ringbuffer_t rx_buf;
#endif

/**
 * @brief Receive a new character from the UART and put it into the receive buffer
 */
void rx_cb(void *arg, char data)
{
    (void)arg;

#ifdef MODULE_UART0
    if (uart0_handler_pid) {
        uart0_handle_incoming(data);

        uart0_notify_thread();
    }
#else
    ringbuffer_add_one(&rx_buf, data);
    mutex_unlock(&uart_rx_mutex);
#endif
}

/**
 * @brief Initialize NewLib, called by __libc_init_array() from the startup script
 */
void _init(void)
{
#ifndef MODULE_UART0
    mutex_init(&uart_rx_mutex);
    ringbuffer_init(&rx_buf, rx_buf_mem, STDIO_RX_BUFSIZE);
#endif
    uart_init(STDIO, STDIO_BAUDRATE, rx_cb, 0, 0);
}

/**
 * @brief Free resources on NewLib de-initialization, not used for RIOT
 */
void _fini(void)
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
    printf("#! exit %i: resetting\n", n);
    NVIC_SystemReset();
    while(1);
}

/**
 * @brief Allocate memory from the heap.
 *
 * The current heap implementation is very rudimentary, it is only able to allocate
 * memory. But it does not
 * - have any means to free memory again
 *
 * @return [description]
 */
caddr_t _sbrk_r(struct _reent *r, ptrdiff_t incr)
{
    unsigned int state = disableIRQ();
    caddr_t res = heap_top;

    if (((incr > 0) && ((heap_top + incr > &_eheap) || (heap_top + incr < res))) ||
        ((incr < 0) && ((heap_top + incr < &_sheap) || (heap_top + incr > res)))) {
        r->_errno = ENOMEM;
        res = (void *) -1;
    }
    else {
        heap_top += incr;
    }

    restoreIRQ(state);
    return res;
}

/**
 * @brief Get the process-ID of the current thread
 *
 * @return      the process ID of the current thread
 */
int _getpid(void)
{
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
int _kill_r(struct _reent *r, int pid, int sig)
{
    (void) pid;
    (void) sig;
    r->_errno = ESRCH;                      /* not implemented yet */
    return -1;
}

/**
 * @brief Open a file
 *
 * @param r     TODO
 * @param name  TODO
 * @param mode  TODO
 *
 * @return      TODO
 */
int _open_r(struct _reent *r, const char *name, int mode)
{
    (void) name;
    (void) mode;
    r->_errno = ENODEV;                     /* not implemented yet */
    return -1;
}

/**
 * @brief Read from a file
 *
 * All input is read from UART_0. The function will block until a byte is actually read.
 *
 * Note: the read function does not buffer - data will be lost if the function is not
 * called fast enough.
 *
 * TODO: implement more sophisticated read call.
 *
 * @param r     TODO
 * @param fd    TODO
 * @param buffer TODO
 * @param int   TODO
 *
 * @return      TODO
 */
int _read_r(struct _reent *r, int fd, void *buffer, unsigned int count)
{
#ifndef MODULE_UART0
    while (rx_buf.avail == 0) {
        mutex_lock(&uart_rx_mutex);
    }
    return ringbuffer_get(&rx_buf, (char*)buffer, rx_buf.avail);
#else
    char *res = (char*)buffer;
    res[0] = (char)uart0_readc();
    return 1;
#endif
}

/**
 * @brief Write characters to a file
 *
 * All output is currently directed to UART_0, independent of the given file descriptor.
 * The write call will further block until the byte is actually written to the UART.
 *
 * TODO: implement more sophisticated write call.
 *
 * @param r     TODO
 * @param fd    TODO
 * @param data  TODO
 * @param int   TODO
 *
 * @return      TODO
 */
int _write_r(struct _reent *r, int fd, const void *data, unsigned int count)
{
    (void) r;
    (void) fd;
    unsigned int i = 0;

    while (i < count) {
        uart_write_blocking(STDIO, ((char*)data)[i++]);
    }

    return (int)i;
}

/**
 * @brief Close a file
 *
 * @param r     TODO
 * @param fd    TODO
 *
 * @return      TODO
 */
int _close_r(struct _reent *r, int fd)
{
    (void) fd;
    r->_errno = ENODEV;                     /* not implemented yet */
    return -1;
}

/**
 * @brief Set position in a file
 *
 * @param r     TODO
 * @param fd    TODO
 * @param pos   TODO
 * @param dir   TODO
 *
 * @return      TODO
 */
_off_t _lseek_r(struct _reent *r, int fd, _off_t pos, int dir)
{
    (void) fd;
    (void) pos;
    (void) dir;
    r->_errno = ENODEV;                     /* not implemented yet */
    return -1;
}

/**
 * @brief Status of an open file
 *
 * @param r     TODO
 * @param fd    TODO
 * @param stat  TODO
 *
 * @return      TODO
 */
int _fstat_r(struct _reent *r, int fd, struct stat *st)
{
    (void) fd;
    (void) st;
    r->_errno = ENODEV;                     /* not implemented yet */
    return -1;
}

/**
 * @brief Status of a file (by name)
 *
 * @param r     TODO
 * @param name  TODO
 * @param stat  TODO
 *
 * @return      TODO
 */
int _stat_r(struct _reent *r, char *name, struct stat *st)
{
    (void) name;
    (void) st;
    r->_errno = ENODEV;                     /* not implemented yet */
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
 * @brief  Remove a file's directory entry
 *
 * @param r     TODO
 * @param path  TODO
 *
 * @return      TODO
 */
int _unlink_r(struct _reent *r, char *path)
{
    (void) path;
    r->_errno = ENODEV;                     /* not implemented yet */
    return -1;
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
int _kill(int pid, int sig)
{
    (void) pid;
    (void) sig;
    errno = ESRCH;                         /* not implemented yet */
    return -1;
}
