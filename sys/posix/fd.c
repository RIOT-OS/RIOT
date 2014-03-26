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

#ifdef CPU_X86
#   include "x86_uart.h"
#endif

static fd_t fd_table[FD_MAX];

static ssize_t stdio_read(int fd, void *buf_, size_t n)
{
    (void) fd;
    char *buf = buf_;
    return uart0_read(buf, n);
}

static ssize_t stdio_write(int fd, const void *buf_, size_t n)
{
#ifndef CPU_X86
    if (n == 0) {
        return 0;
    }

    (void) fd;
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
#else
    (void) fd;
    return x86_uart_write(buf_, n);
#endif
}

static const fd_ops_t stdio_fd_ops = {
    .read = stdio_read,
    .write = stdio_write,
};

int fd_init(void)
{
    for (unsigned i = 0; i < 3; ++i) {
        fd_table[i].ops = &stdio_fd_ops;
        fd_table[i].fd = i;
    }
    return FD_MAX;
}

int fd_new(int internal_fd, const fd_ops_t *internal_ops)
{
    unsigned old_state = disableIRQ();

    int result = -ENFILE;
    for (int i = 0; i < FD_MAX; i++) {
        if (!fd_table[i].ops) {
            result = i;

            fd_table[i].fd = internal_fd;
            fd_table[i].ops = internal_ops;

            break;
        }
    }

    restoreIRQ(old_state);
    return result;
}

fd_t *fd_get(int fd)
{
    if ((fd >= 0) && (fd < FD_MAX) && fd_table[fd].ops) {
        return &fd_table[fd];
    }

    return NULL;
}

int close(int fildes)
{
    fd_t *fd_obj = fd_get(fildes);

    if (!fd_obj) {
        errno = EBADF;
        return -1;
    }

    if (fd_obj->ops->close && (fd_obj->ops->close(fd_obj->fd) != 0)) {
        errno = EIO;    // EINTR may not occur since RIOT has no signals yet.
        return -1;
    }
    fd_obj->ops = NULL;

    return 0;
}

/**
 * @}
 */
