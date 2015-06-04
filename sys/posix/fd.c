/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup posix
 * @{
 * @file
 * @brief   Providing unifying file descriptor wrapper for POSIX-compliant
 *          operations.
 * @author  Christian Mehlis <mehlis@inf.fu-berlin.de>
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "posix_io.h"
#include "unistd.h"

#include "fd.h"

#ifdef CPU_MSP430
#define FD_MAX 5
#else
#define FD_MAX 15
#endif

static fd_t fd_table[FD_MAX];

int fd_init(void)
{
    memset(fd_table, 0, sizeof(fd_t) * FD_MAX);

    return FD_MAX;
}

static int fd_get_next_free(void)
{
    for (int i = 0; i < FD_MAX; i++) {
        fd_t *cur = &fd_table[i];

        if (!cur->internal_active) {
            return i;
        }
    }

    return -1;
}

int fd_new(int internal_fd, ssize_t (*internal_read)(int, void *, size_t),
           ssize_t (*internal_write)(int, const void *, size_t),
           int (*internal_close)(int))
{
    int fd = fd_get_next_free();

    if (fd >= 0) {
        fd_t *fd_s = fd_get(fd);
        fd_s->internal_active = 1;
        fd_s->internal_fd = internal_fd;
        fd_s->read = internal_read;
        fd_s->write = internal_write;
        fd_s->close = internal_close;
    }
    else {
        errno = ENFILE;
        return -1;
    }

    return fd;
}

fd_t *fd_get(int fd)
{
    if (fd >= 0 && fd < FD_MAX) {
        return &fd_table[fd];
    }

    return NULL;
}

void fd_destroy(int fd)
{
    fd_t *cur = fd_get(fd);

    if (!cur) {
        return;
    }

    memset(cur, 0, sizeof(fd_t));
}

/**
 * @}
 */
