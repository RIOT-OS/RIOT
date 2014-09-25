/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_samd21
 * @{
 *
 * @file        syscalls.c
 * @brief       NewLib system calls implementations for samd21
 *
 * @author      Michael Baar <michael.baar@fu-berlin.de>
 * @author      Stefan Pfeiffer <pfeiffer@inf.fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
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
 * manage the heap
 */
extern uint32_t _end;                       /* address of last used memory cell */
caddr_t heap_top = (caddr_t)&_end + 4;

#ifndef MODULE_UART0
/**
 * @brief use mutex for waiting on incoming UART chars
 */
static mutex_t uart_rx_mutex;
static char rx_buf_mem[STDIO_BUFSIZE];
static ringbuffer_t rx_buf;
#endif

/**
 * @brief Receive a new character from the UART and put it into the receive buffer
 */
void rx_cb(void *arg, char data)
{
#ifndef MODULE_UART0
    (void)arg;

    ringbuffer_add_one(&rx_buf, data);
    mutex_unlock(&uart_rx_mutex);
#else
    if (uart0_handler_pid) {
        uart0_handle_incoming(data);

        uart0_notify_thread();
    }
#endif
}

/**
 * @brief Initialize NewLib, called by __libc_init_array() from the startup script
 */
void _init(void)
{
#ifndef MODULE_UART0
    mutex_init(&uart_rx_mutex);
    ringbuffer_init(&rx_buf, rx_buf_mem, STDIO_BUFSIZE);
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
 * - check if the returned address is valid (no check if the memory very exists)
 * - have any means to free memory again
 *
 * TODO: check if the requested memory is really available
 *
 * @return [description]
 */
caddr_t _sbrk_r(struct _reent *r, size_t incr)
{
    unsigned int state = disableIRQ();
    caddr_t res = heap_top;
    heap_top += incr;
    restoreIRQ(state);
    return res;
}

/**
 * @brief Get the process-ID of the current thread
 *
 * @return      the process ID of the current thread
 */
pid_t _getpid(void)
{
    return (pid_t) sched_active_pid;
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
int _kill_r(struct _reent *r, pid_t pid, int sig)
{
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
    r->_errno = ENODEV;
    return -1;
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
    char *c = (char*)data;
    for (int i = 0; i < count; i++) {
        uart_write_blocking(UART_0, c[i]);
    }
    return count;
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
int _fstat_r(struct _reent *r, int fd, struct stat * st)
{
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
    if(fd == STDOUT_FILENO || fd == STDERR_FILENO) {
        return 1;
    }
    else {
        return 0;
    }
}

/**
 * @brief  Remove a file's directory entry
 *
 * @param r     TODO
 * @param path  TODO
 *
 * @return      TODO
 */
int _unlink_r(struct _reent *r, char* path)
{
    r->_errno = ENODEV;                     /* not implemented yet */
    return -1;
}
