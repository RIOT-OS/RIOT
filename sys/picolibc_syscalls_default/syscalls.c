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
void _exit(int n)
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
FILE picolibc_stdout =
    FDEV_SETUP_STREAM(picolibc_put, NULL, NULL, _FDEV_SETUP_WRITE);

FILE picolibc_stdin =
    FDEV_SETUP_STREAM(NULL, picolibc_get, NULL, _FDEV_SETUP_READ);

FILE *const __iob[] = {
    &picolibc_stdin,
    &picolibc_stdout,
    &picolibc_stdout,
};

/*
 * All output is directed to stdio_uart, independent of the given file descriptor.
 * The write call will further block until the byte is actually written to the UART.
 */
_READ_WRITE_RETURN_TYPE write(int fd, const void *data, size_t count)
{
    (void) fd;
    return stdio_write(data, count);
}

/**
 * @brief Get the process-ID of the current thread
 *
 * @return      the process ID of the current thread
 */
pid_t getpid(void)
{
    return sched_active_pid;
}

int close(int fd)
{
    (void) fd;
    errno = ENODEV;
    return -1;
}
