/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @file    fd.c
 * @brief   Providing implementation for close for fds defined in fd.h.
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 * @author  Christian Mehlis <mehlis@inf.fu-berlin.de>
 */
#include <errno.h>

#include "fd.h"
#include "unistd.h"

int close(int fildes)
{
    fd_t *fd_obj = fd_get(fildes);

    if (!fd_obj) {
        errno = EBADF;
        return -1;
    }

<<<<<<< HEAD
    if (fd_obj->close(fd_obj->fd) < 0) {
=======
    if (fd_obj->close(fd_obj->internal_fd) < 0) {
>>>>>>> upstream/master
        errno = EIO;    // EINTR may not occur since RIOT has no signals yet.
        return -1;
    }

<<<<<<< HEAD
    fd_destroy(fd_obj->fd);
=======
    fd_destroy(fd_obj->internal_fd);
>>>>>>> upstream/master

    return 0;
}

int usleep(useconds_t useconds)
{
    timex_t time = timex_set(0, useconds);
    timex_normalize(&time);
    vtimer_sleep(time);
    return 0;
}

unsigned int sleep(unsigned int seconds)
{
    timex_t time = timex_set(seconds, 0);
    vtimer_sleep(time);
    return 0;
}

/**
 * @}
 */
