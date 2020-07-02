/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @file
 * @author  Martine S. Lenders <m.lenders@fu-berlin.de>
 */

#include <errno.h>
#include <stdbool.h>
#include <sys/select.h>

#include "thread_flags.h"
#include "vfs.h"
#include "xtimer.h"

#if IS_USED(MODULE_POSIX_SOCKETS)
extern bool posix_socket_is(int fd);
extern unsigned posix_socket_avail(int fd);
extern void posix_socket_select(int fd);
#else   /* MODULE_POSIX_SOCKETS */
static inline bool posix_socket_is(int fd)
{
    (void)fd;
    return false;
}

static inline unsigned posix_socket_avail(int fd)
{
    (void)fd;
    return 0;
}

static inline void posix_socket_select(int fd)
{
    (void)fd;
    return 0;
}
#endif  /* IS_USED(MODULE_POSIX_SOCKETS) */

static int _set_timeout(xtimer_t *timeout_timer, struct timeval *timeout,
                        uint32_t offset, bool *wait)
{
    if (timeout != NULL) {
        uint64_t t = ((uint64_t)(timeout->tv_sec * US_PER_SEC) +
                      timeout->tv_usec);
        /* check for potential underflow before subtracting offset */
        if ((t == 0) || (offset > t)) {
            *wait = false;
            return 0;
        }
        t -= offset;
        if (t > UINT32_MAX) {
            errno = EINVAL;
            /* don't have timer set yet so go to end */
            return -1;
        }
        else {
            xtimer_set_timeout_flag(timeout_timer, (uint32_t)t);
        }
    }
    return 0;
}

int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *errorfds,
           struct timeval *timeout)
{
    uint32_t start_time = xtimer_now_usec();
    fd_set ret_readfds;
    xtimer_t timeout_timer;
    int fds_set = 0;
    bool wait = true;

    FD_ZERO(&ret_readfds);
    /* TODO ignored writefds and errorfds for now since there is no point for
     * them with sockets */
    if ((nfds >= FD_SETSIZE) || ((unsigned)nfds >= VFS_MAX_OPEN_FILES)) {
        errno = EINVAL;
        return -1;
    }
    for (int i = 0; i < nfds; i++) {
        if ((readfds != NULL) && FD_ISSET(i, readfds)) {
            if (!posix_socket_is(i)) {
                errno = EBADF;
                return -1;
            }
            if (posix_socket_avail(i) > 0) {
                FD_SET(i, &ret_readfds);
                fds_set++;
                wait = false;
            }
            else {
                posix_socket_select(i);
            }
        }
        if ((writefds != NULL) && FD_ISSET(i, writefds) &&
            !posix_socket_is(i)) {
            errno = EBADF;
            return -1;
        }
        if ((errorfds != NULL) && FD_ISSET(i, errorfds) &&
            !posix_socket_is(i)) {
            errno = EBADF;
            return -1;
        }
    }
    while (wait) {
        if (_set_timeout(&timeout_timer, timeout,
                         xtimer_now_usec() - start_time, &wait) < 0) {
            return -1;
        }
        if (!wait) {
            errno = EINTR;
            return -1;
        }
        thread_flags_t tflags = thread_flags_wait_any(POSIX_SELECT_THREAD_FLAG |
                                                      THREAD_FLAG_TIMEOUT);
        if (tflags & POSIX_SELECT_THREAD_FLAG) {
            for (int i = 0; i < nfds; i++) {
                if (FD_ISSET(i, readfds)) {
                    if (posix_socket_avail(i) > 0) {
                        FD_SET(i, &ret_readfds);
                        fds_set++;
                        wait = false;
                    }
                }
            }
        }
        else if (tflags & THREAD_FLAG_TIMEOUT) {
            errno = EINTR;
            return -1;
        }
        xtimer_remove(&timeout_timer);
    }
    *readfds = ret_readfds;
    return fds_set;
}
