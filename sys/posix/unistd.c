/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @file    unistd.c
 * @brief   Providing implementation for close for fds defined in fd.h.
 * @author  Martin Lenders <mlenders@inf.fu-berlin.de>
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

    if (fd_obj->close(fd_obj->fd) < 0) {
        errno = EIO;    // EINTR may not occur since RIOT has no signals yet.
        return -1;
    }

    fd_destroy(fd_obj->fd);

    return 0;
}

int usleep(useconds_t useconds)
{
    if (useconds == 0) {
        return 0;
    }

    int result = vtimer_usleep((uint32_t) useconds);
    if (result == 0) {
        return 0;
    }
    else if (result > 0) {
        /* The sleep was interrupted. The thread was woken up spuriously. */
        errno = EINTR;
        return -1;
    }
    else /* if (result < 0) */ {
        /* The sleep could not be performed. vtimer_set() returned an error value. */
        errno = EINVAL;
        return -1;
    }
}

unsigned int sleep(unsigned int seconds)
{
    if (seconds == 0) {
        return 0;
    }

    timex_t now;
    vtimer_now(&now);
    timex_t timestamp = timex_set(seconds, 0);
    timestamp = timex_add(now, timestamp);

    int result = vtimer_sleep(timestamp, VTIMER_ABSOLUTE);
    if (result == 0) {
        return 0;
    }
    else if (result > 0) {
        /* The sleep was interrupted. The thread was woken up spuriously. */
        timex_t now_after;
        vtimer_now(&now_after);
        now_after = timex_sub(now_after, now);
        return now_after.seconds;
    }
    else /* if (result < 0) */ {
        /* The sleep could not be performed. vtimer_set() returned an error value. */
        return seconds;
    }
}

/**
 * @}
 */
