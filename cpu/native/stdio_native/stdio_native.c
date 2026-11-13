/*
 * SPDX-FileCopyrightText: 2019 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @file
 * @author Martine S. Lenders <m.lenders@fu-berlin.de>
 * @author Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#include <stdbool.h>

#include "async_read.h"
#include "irq.h"
#include "kernel_defines.h"
#include "macros/utils.h"
#include "native_internal.h"

#include "stdio_base.h"

/**
 * @brief   Minimum free space in stdin before reading is resumed
 *
 * This serves as a threshold to prevent pausing and resuming too often due to
 * small increments in free space in stdin.
 */
#define RESUME_THRESHOLD (STDIO_RX_BUFSIZE / 2)

static bool _rx_paused;

static void _resume(void *arg)
{
    (void)arg;

    /* this runs in the context of any thread that reads from stdin, so signals
     * are blocked to make the check-and-clear of the pause state atomic, and
     * to prevent a context switch while signalling the async read child */
    unsigned irqstate = irq_disable();

    if (_rx_paused && (stdio_stdin_get_free() >= RESUME_THRESHOLD)) {
        _rx_paused = false;
        _native_pending_syscalls_up();
        native_async_read_continue(STDIN_FILENO);
        _native_pending_syscalls_down
    }

    irq_restore(irqstate);
}

static void _async_read_wrapper(int fd, void *arg)
{
    (void)arg;

    uint8_t buf[STDIO_RX_BUFSIZE];
    size_t len = sizeof(buf);

    /* only read into stdin what fits, the rest will be buffered by the host */
    if (IS_USED(MODULE_STDIO_STDIN_RESUME)) {
        len = MIN(len, stdio_stdin_get_free());
    }

    if (len > 0) {
        int res = real_read(fd, buf, len);
        if (res > 0) {
            stdio_rx_write(buf, res);
        }
    }

    /* reading is paused if stdin is full */
    if (IS_USED(MODULE_STDIO_STDIN_RESUME) && (stdio_stdin_get_free() == 0)) {
        _rx_paused = true;
        return;
    }

    _native_pending_syscalls_up();
    native_async_read_continue(fd);
    _native_pending_syscalls_down();
}

static void _init(void)
{
    native_async_read_setup();
    if (IS_USED(MODULE_STDIN)) {
        if (IS_USED(MODULE_STDIO_STDIN_RESUME)) {
            stdio_stdin_resume_set(_resume, NULL);
        }
        native_async_read_add_int_handler(STDIN_FILENO, NULL,
                                          _async_read_wrapper);
    }
}

static ssize_t _write(const void* buffer, size_t len)
{
    return real_write(STDOUT_FILENO, buffer, len);
}

STDIO_PROVIDER(STDIO_NATIVE, _init, NULL, _write)

/** @} */
