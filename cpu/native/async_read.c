/**
 * Multiple asynchronus read on file descriptors
 *
 * Copyright (C) 2015 Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>,
 *                    Martine Lenders <mlenders@inf.fu-berlin.de>
 *                    Kaspar Schleiser <kaspar@schleiser.de>
 *                    Ell-i open source co-operative
 *                    Takuo Yonezawa <Yonezawa-T2@mail.dnp.co.jp>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup native_cpu
 * @{
 * @file
 * @author  Takuo Yonezawa <Yonezawa-T2@mail.dnp.co.jp>
 */

#include <err.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "async_read.h"
#include "native_internal.h"

static int _next_index;
static int _fds[ASYNC_READ_NUMOF];
static void *_args[ASYNC_READ_NUMOF];
static native_async_read_callback_t _native_async_read_callbacks[ASYNC_READ_NUMOF];

#ifdef __MACH__
static pid_t _sigio_child_pids[ASYNC_READ_NUMOF];
static void _sigio_child(int fd);
#endif

static void _async_io_isr(void) {
    fd_set rfds;

    FD_ZERO(&rfds);

    int max_fd = 0;

    struct timeval timeout = { .tv_usec = 0 };

    for (int i = 0; i < _next_index; i++) {
        FD_SET(_fds[i], &rfds);

        if (max_fd < _fds[i]) {
            max_fd = _fds[i];
        }
    }

    if (real_select(max_fd + 1, &rfds, NULL, NULL, &timeout) > 0) {
        for (int i = 0; i < _next_index; i++) {
            if (FD_ISSET(_fds[i], &rfds)) {
                _native_async_read_callbacks[i](_fds[i], _args[i]);
            }
        }
    }
}

void native_async_read_setup(void) {
    register_interrupt(SIGIO, _async_io_isr);
}

void native_async_read_cleanup(void) {
    unregister_interrupt(SIGIO);

    for (int i = 0; i < _next_index; i++) {
#ifdef __MACH__
        kill(_sigio_child_pids[i], SIGKILL);
#endif
        real_close(_fds[i]);
    }
}

void native_async_read_continue(int fd) {
    (void) fd;
#ifdef __MACH__
    for (int i = 0; i < _next_index; i++) {
        if (_fds[i] == fd) {
            kill(_sigio_child_pids[i], SIGCONT);
        }
    }
#endif
}

void native_async_read_add_handler(int fd, void *arg, native_async_read_callback_t handler) {
    if (_next_index >= ASYNC_READ_NUMOF) {
        err(EXIT_FAILURE, "native_async_read_add_handler(): too many callbacks");
    }

    _fds[_next_index] = fd;
    _args[_next_index] = arg;
    _native_async_read_callbacks[_next_index] = handler;

#ifdef __MACH__
    /* tuntap signalled IO is not working in OSX,
     * * check http://sourceforge.net/p/tuntaposx/bugs/17/ */
    _sigio_child(_next_index);
#else
    /* configure fds to send signals on io */
    if (fcntl(fd, F_SETOWN, _native_pid) == -1) {
        err(EXIT_FAILURE, "native_async_read_add_handler(): fcntl(F_SETOWN)");
    }
    /* set file access mode to non-blocking */
    if (fcntl(fd, F_SETFL, O_NONBLOCK | O_ASYNC) == -1) {
        err(EXIT_FAILURE, "native_async_read_add_handler(): fcntl(F_SETFL)");
    }
#endif /* not OSX */

    _next_index++;
}

#ifdef __MACH__
static void _sigio_child(int index)
{
    int fd = _fds[index];
    pid_t parent = _native_pid;
    pid_t child;
    if ((child = real_fork()) == -1) {
        err(EXIT_FAILURE, "sigio_child: fork");
    }
    if (child > 0) {
        _sigio_child_pids[index] = child;

        /* return in parent process */
        return;
    }

    sigset_t sigmask;

    sigemptyset(&sigmask);
    sigaddset(&sigmask, SIGCONT);
    sigprocmask(SIG_BLOCK, &sigmask, NULL);

    /* watch tap interface and signal parent process if data is
     * available */
    fd_set rfds;
    while (1) {
        FD_ZERO(&rfds);
        FD_SET(fd, &rfds);
        if (real_select(fd + 1, &rfds, NULL, NULL, NULL) == 1) {
            kill(parent, SIGIO);
        }
        else {
            kill(parent, SIGKILL);
            err(EXIT_FAILURE, "osx_sigio_child: select");
        }

        /* If SIGCONT is sent before calling pause(), the process stops
         * forever, so using sigwait instead. */

        int sig;

        sigemptyset(&sigmask);
        sigaddset(&sigmask, SIGCONT);
        sigwait(&sigmask, &sig);
    }
}
#endif
/** @} */
