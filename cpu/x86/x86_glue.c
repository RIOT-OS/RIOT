/*
 * Copyright (C) 2014  René Kijewski  <rene.kijewski@fu-berlin.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

/**
 * @ingroup     x86
 * @{
 *
 * @file
 * @brief       The interface between newlib and kernel functions.
 *
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 *
 * @}
 */

#include "attributes.h"
#include "cpu.h"
#include "kernel_internal.h"
#include "sched.h"
#include "x86_uart.h"

#ifdef MODULE_UART0
#   include "board_uart0.h"
#endif

#include <signal.h>
#include <sys/stat.h>
#include <unistd.h>

void _exit(int status)
{
    (void) status;
    sched_task_exit();
    UNREACHABLE();
}

#ifndef MODULE_POSIX
/* Already defined in /sys/posix/unistd.c */

int close(int fildes)
{
    /* TODO */
    (void) fildes;
    return -1;
}
#endif

pid_t getpid(void)
{
    return (pid_t) sched_active_pid;
}

int fstat(int fildes, struct stat *buf)
{
    /* TODO */
    (void) fildes;
    (void) buf;
    return -1;
}

int isatty(int fildes)
{
    /* TODO */
    (void) fildes;
    return 0; /* sic */
}

int kill(pid_t pid, int sig)
{
    /* TODO */
    (void) pid;
    (void) sig;
    return -1;
}

off_t lseek(int fildes, off_t offset, int whence)
{
    /* TODO */
    (void) fildes;
    (void) offset;
    (void) whence;
    return (off_t) -1;
}

ssize_t read(int fildes, void *buf, size_t nbyte)
{
    if (nbyte == 0) {
        /* allow reading nothing from every FD ... */
        return 0;
    }
    else if (fildes == STDOUT_FILENO || fildes == STDERR_FILENO) {
        /* cannot read from STDOUT or STDERR */
        return -1;
    }
    else if (fildes == STDIN_FILENO) {
#ifdef MODULE_UART0
        return uart0_read(buf, nbyte);
#else
        return x86_uart_read(buf, nbyte);
#endif
    }

    /* TODO: find appropriate FILE */
    (void) fildes;
    (void) buf;
    (void) nbyte;
    return -1;
}

ssize_t write(int fildes, const void *buf, size_t nbyte)
{
    if (nbyte == 0) {
        /* allow writing nothing to every FD ... */
        return 0;
    }
    else if (fildes == STDIN_FILENO) {
        /* cannot write to STDIN */
        return -1;
    }
    else if (fildes == STDOUT_FILENO || fildes == STDERR_FILENO) {
        return x86_uart_write(buf, nbyte);
    }

    /* TODO: find appropriate FILE */
    (void) fildes;
    (void) buf;
    (void) nbyte;
    return -1;
}

