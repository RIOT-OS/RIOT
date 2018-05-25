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
#include <poll.h>

#include "async_read.h"
#include "native_internal.h"

static int _next_index;
static struct pollfd _fds[ASYNC_READ_NUMOF];
static void *_args[ASYNC_READ_NUMOF];
static native_async_read_callback_t _native_async_read_callbacks[ASYNC_READ_NUMOF];

static pid_t _sigio_child_pids[ASYNC_READ_NUMOF];
static void _sigio_child(int fd);

static void _async_io_isr(void) {
    int max_fd = 0;

    if (real_poll(_fds, max_fd + 1, 0) > 0) {
        for (int i = 0; i < _next_index; i++) {
            if (_fds[i].revents & POLLIN) { 
                _native_async_read_callbacks[i](_fds[i].fd, _args[i]);
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
        kill(_sigio_child_pids[i], SIGKILL);
    }
}

void native_async_read_continue(int fd) {
    (void) fd;
    for (int i = 0; i < _next_index; i++) {
        if (_fds[i].fd == fd) {
            kill(_sigio_child_pids[i], SIGCONT);
        }
    }
}

void native_async_read_add_handler(int fd, void *arg, native_async_read_callback_t handler) {
    if (_next_index >= ASYNC_READ_NUMOF) {
        err(EXIT_FAILURE, "native_async_read_add_handler(): too many callbacks");
    }

    _fds[_next_index].fd = fd;
    _fds[_next_index].events = POLLIN;
    _args[_next_index] = arg;
    _native_async_read_callbacks[_next_index] = handler;

    /* tuntap signalled IO is not working in OSX,
     * * check http://sourceforge.net/p/tuntaposx/bugs/17/ */
    _sigio_child(_next_index);
    _next_index++;
}

void native_async_interrupt_add_handler(int fd, void *arg, native_async_read_callback_t handler) {
    if (_next_index >= ASYNC_READ_NUMOF) {
        err(EXIT_FAILURE, "native_async_read_add_handler(): too many callbacks");
    }

    _fds[_next_index].fd = fd;
    _fds[_next_index].events = POLLPRI;
    _args[_next_index] = arg;
    _native_async_read_callbacks[_next_index] = handler;

    _sigio_child(_next_index);
    _next_index++;
}

static void _sigio_child(int index)
{
    struct pollfd fds = _fds[index];
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
    while (1) {
        if (real_poll(&fds, 1, -1) == 1) {
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
/** @} */
