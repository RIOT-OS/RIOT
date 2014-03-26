/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup posix
 * @{
 * @file    fd.c
 * @brief   Providing unifying file descriptor wrapper for POSIX-compliant
 *          operations.
 * @author  Christian Mehlis <mehlis@inf.fu-berlin.de>
 * @author  Martin Lenders <mlenders@inf.fu-berlin.de>
 * @author  René Kijewski <rene.kijewski@fu-berlin.de>
 */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "posix_io.h"
#include "board_uart0.h"
#include "unistd.h"
#include "irq.h"

#include "fd.h"

#ifdef CPU_MSP430
#   define FD_MAX 5
#else
#   define FD_MAX 15
#endif

#if defined (CPU_X86)
#   include "x86_uart.h"
#elif defined (CPU_NATIVE)
#   include "native_internal.h"
#endif

#define FD_TABLE_OFFSET 3

static fd_t fd_table[FD_MAX - FD_TABLE_OFFSET];

static ssize_t stdio_write(int fd, const void *buf_, size_t n)
{
    (void) fd;
#if defined (CPU_X86)
    return x86_uart_write(buf_, n);
#elif defined (CPU_NATIVE)
    return _native_write(fd, buf_, n);
#else
    if (n == 0) {
        return 0;
    }

    const char *buf = buf_;

    ssize_t wrote = 0;
    while (n > 0) {
        int put = putchar(*buf);
        if (put == *buf) {
            ++wrote;
            ++buf;
            --n;
        }
        else if (wrote > 0) {
            return wrote;
        }
        else {
            return -1;
        }
    }
    return wrote;
#endif
}

int fd_new(int internal_fd, const fd_ops_t *internal_ops)
{
    unsigned old_state = disableIRQ();

    int result = -ENFILE;
    for (int i = FD_TABLE_OFFSET; i < FD_MAX; i++) {
        if (!fd_table[i - FD_TABLE_OFFSET].ops) {
            result = i;

            fd_table[i - FD_TABLE_OFFSET].fd = internal_fd;
            fd_table[i - FD_TABLE_OFFSET].ops = internal_ops;

            break;
        }
    }

    restoreIRQ(old_state);
    return result;
}

static fd_t *fd_get(int fd)
{
    if ((fd >= FD_TABLE_OFFSET) && (fd < FD_MAX) && fd_table[fd - FD_TABLE_OFFSET].ops) {
        return &fd_table[fd - FD_TABLE_OFFSET];
    }

    return NULL;
}

ssize_t read(int fildes, void *buf, size_t n)
{
    if (fildes == STDIN_FILENO) {
        return uart0_read(buf, n);
    }

    fd_t *fd_obj = fd_get(fildes);

    if (!fd_obj || !fd_obj->ops->read) {
        errno = EBADF;
        return -1;
    }

    ssize_t result = fd_obj->ops->read(fd_obj->fd, buf, n);
    if (result < 0) {
        errno = EIO; // EINTR may not occur since RIOT has no signals yet.
    }
    return result;
}

ssize_t write(int fildes, const void *buf, size_t n)
{
    if ((fildes == STDOUT_FILENO) || (fildes == STDERR_FILENO)) {
        return stdio_write(fildes, buf, n);
    }

    fd_t *fd_obj = fd_get(fildes);

    if (!fd_obj || !fd_obj->ops->write) {
        errno = EBADF;
        return -1;
    }

    ssize_t result = fd_obj->ops->write(fd_obj->fd, buf, n);
    if (result < 0) {
        errno = EIO; // EINTR may not occur since RIOT has no signals yet.
    }
    return result;
}

int close(int fildes)
{
    fd_t *fd_obj = fd_get(fildes);

    if (!fd_obj) {
        errno = EBADF;
        return -1;
    }

    if (fd_obj->ops->close && (fd_obj->ops->close(fd_obj->fd) != 0)) {
        errno = EIO; // EINTR may not occur since RIOT has no signals yet.
        return -1;
    }
    fd_obj->ops = NULL;

    return 0;
}

/**
 * @}
 */
