/*
 * Copyright (C) 2014 Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup native_cpu
 * @{
 * @file
 * @author  Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 * @}
 */

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <err.h>
#include <string.h>

#include <poll.h>

#include "irq.h"

#include "native_internal.h"
#include "native_fildes.h"

#define ENABLE_DEBUG (1)
#include "debug.h"

#define NATIVE_MAX_POLLFDS  (32)

/********************************************************************************
 * internal variable declarations
 *******************************************************************************/

/** all of natives open file descriptors used for pollin */
static struct pollfd _native_pfds[NATIVE_MAX_POLLFDS];

/** number of file descriptors currently in use for polling */
static volatile nfds_t _native_npfds;

/** native fd storage type declarition */
typedef struct {
    int fd;         /**< the host fd */
    int pf_idx;     /**< index in _native_pfds, or -1 if unset */
    short events;   /**< event mask */
    /** event handlers (NULL if unset) */
    _native_callback_t handlers[_ev_pollnval + 1];
} _native_fd_storage_t;

/** all file descriptors in use by native */
static _native_fd_storage_t _native_fds[NATIVE_MAX_POLLFDS];
/** total number of file descriptors in use by native */
static volatile unsigned _native_nfds;

/********************************************************************************
 * internal API declaration
 *******************************************************************************/

static int add_fd_to_pfd(_native_fd_t nfd);
static void _native_fd_signal_handler(void);

/********************************************************************************
 * "public" API implementation
 *******************************************************************************/

_native_fd_t _native_fd_add(int fd)
{
    /* enter critical section */
    unsigned state = disableIRQ();

    /* make sure there is enough space */
    if (_native_nfds >= NATIVE_MAX_POLLFDS) {
        errx(EXIT_FAILURE, "_native_pfd_add_fd: not enough space, try increasing NATIVE_MAX_POLLFDS");
        return -1; /* actually, errx does not return */
    }

    /* get next free fd enumerator */
    _native_fd_t nfd = _native_nfds++;

    /* initialize the structs */
    _native_fds[nfd].fd = fd;
    _native_fds[nfd].events = 0;
    memset(&_native_fds[nfd].handlers, 0x0,
            sizeof(_native_fds[nfd].handlers));
    _native_fds[nfd].pf_idx = -1;

    /* leave critical section */
    restoreIRQ(state);

    return nfd;
}

int _native_fd_remove(_native_fd_t fd)
{
    unsigned state = disableIRQ();

    /* make sure the fd exists */
    if ((fd < 0) || ((unsigned)fd > _native_nfds)) {
        errx(EXIT_FAILURE, "_native_fd_remove: invalid fd");
        restoreIRQ(state);
        return -1;
    }

    _native_fds[fd].pf_idx = -1;
    _native_fds[fd].fd = -1;
    _native_fds[fd].events = 0;
    memset(&_native_fds[fd].handlers, 0x0,
            sizeof(_native_fds[fd].handlers));

    restoreIRQ(state);
    return 0;
}

int _native_fd_set_handler(_native_fd_t fd, _native_fd_event_t event, _native_callback_t handler)
{
    /* make sure the fd exists */
    if ((fd < 0) || ((unsigned)fd > _native_nfds)) {
        errx(EXIT_FAILURE, "_native_pfd_set_handler: invalid fd");
        return -1;
    }

    /* configure the structs */
    _native_fds[fd].handlers[event] = handler;
    switch (event) {
        case _ev_pollin:
            _native_fds[fd].events |= POLLIN;
            break;
        case _ev_pollrdnorm:
            _native_fds[fd].events |= POLLRDNORM;
            break;
        case _ev_pollrdband:
            _native_fds[fd].events |= POLLRDBAND;
            break;
        case _ev_pollpri:
            _native_fds[fd].events |= POLLPRI;
            break;
        case _ev_pollout:
            _native_fds[fd].events |= POLLOUT;
            break;
        case _ev_pollwrband:
            _native_fds[fd].events |= POLLWRBAND;
            break;
        case _ev_pollerr:
            _native_fds[fd].events |= POLLERR;
            break;
        case _ev_pollhup:
            _native_fds[fd].events |= POLLHUP;
            break;
        case _ev_pollnval:
            _native_fds[fd].events |= POLLNVAL;
            break;
    }

    if (add_fd_to_pfd(fd) != 0) {
        errx(EXIT_FAILURE, "_native_pfd_set_handler: kaputt\n");
        return -1; /* errx does not return */
    }

    return 0;
}

int _native_fd_close_all(void)
{
    int ret = 0;
    for (unsigned i = 0; i < _native_nfds; i++) {
        if (real_close(_native_fds[i].fd) == -1) {
            warn("_native_fd_close_all: close(_native_nfds[%i].fd)", i);
            ret = -1;
        }
    }
    return ret;
}

void _native_fd_init(void)
{
    printf("_native_fd_init\n");
    register_interrupt(SIGIO, _native_fd_signal_handler);
}

/********************************************************************************
 * internal API implementation
 *******************************************************************************/

static void _native_handle_events(int pfd)
{
    int fd = -1;

    for (unsigned i = 0; i < _native_nfds; i++) {
        if (_native_fds[i].pf_idx == pfd) {
            fd = i;
            break;
        }
    }
    if (fd == -1) {
        errx(EXIT_FAILURE, "_native_handle_events: internal error");
    }

    printf("_native_handle_events\n");

    short revent = _native_pfds[pfd].revents;
    _native_callback_t *handlers = _native_fds[fd].handlers;

    if (revent & POLLIN) {
        printf("handling _ev_pollin\n");
        handlers[_ev_pollin]();
    }
    if (revent & POLLRDNORM) {
        printf("handling _ev_pollrdnorm\n");
        handlers[_ev_pollrdnorm]();
    }
    if (revent & POLLRDBAND) {
        printf("handling _ev_pollrdband\n");
        handlers[_ev_pollrdband]();
    }
    if (revent & POLLPRI) {
        printf("handling _ev_pollpri\n");
        handlers[_ev_pollpri]();
    }
    if (revent & POLLOUT) {
        printf("handling _ev_pollout\n");
        handlers[_ev_pollout]();
    }
    if (revent & POLLWRBAND) {
        printf("handling _ev_pollwrband\n");
        handlers[_ev_pollwrband]();
    }
    if (revent & POLLERR) {
        printf("handling _ev_pollerr\n");
        handlers[_ev_pollerr]();
    }
    if (revent & POLLHUP) {
        printf("handling _ev_pollhup\n");
        handlers[_ev_pollhup]();
    }
    if (revent & POLLNVAL) {
        printf("handling _ev_pollnval\n");
        handlers[_ev_pollnval]();
    }
}

static void _native_fd_signal_handler(void)
{
    printf("_native_fd_signal_handler checking %lu pfds\n", (unsigned long) _native_npfds);

    int ready = real_poll(_native_pfds, _native_npfds, 0);
    if (ready == -1) {
        err(EXIT_FAILURE, "_native_fd_signal_handler: poll");
    }
    printf("_native_fd_signal_handler: %i ready\n", ready);

    for (unsigned i = 0; i < _native_npfds; i++) {
        if (_native_pfds[i].revents) {
            _native_handle_events(i);
        }
    }
}

static int add_fd_to_pfd(_native_fd_t nfd)
{
    printf("add_fd_to_pfd\n");
    unsigned state = disableIRQ();

    int idx = _native_fds[nfd].pf_idx;
    if (idx == -1) {
        /* make sure there is enough space */
        if (_native_npfds >= NATIVE_MAX_POLLFDS) {
            errx(EXIT_FAILURE, "add_fd_to_pfd: not enough space, try increasing NATIVE_MAX_POLLFDS");
            return -1; /* errx does not return */
        }

        /* get next free fd enumerator */
        idx = _native_npfds++;

        _native_fds[nfd].pf_idx = idx;

        printf("add_fd_to_pfd: now handling %lu pfds\n", (unsigned long) _native_npfds);
    }

    _native_pfds[idx].fd = _native_fds[nfd].fd;
    _native_pfds[idx].events = _native_fds[nfd].events;

    restoreIRQ(state);

    return 0;
}
