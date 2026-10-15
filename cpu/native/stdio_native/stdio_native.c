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

    if (_rx_paused && (stdio_rx_free() >= RESUME_THRESHOLD)) {
        _rx_paused = false;
        native_async_read_continue(STDIN_FILENO);
    }
}

static void _async_read_wrapper(int fd, void *arg)
{
    (void)arg;

    uint8_t buf[STDIO_RX_BUFSIZE];
    size_t len = sizeof(buf);

    /* only read into stdin what fits, the rest will be buffered by the host */
    if (IS_USED(MODULE_STDIO_RESUME)) {
        len = stdio_rx_free();
    }

    if (len > 0) {
        int res = real_read(fd, buf, len);
        if (res > 0) {
            stdio_rx_write(buf, res);
        }
    }

    /* reading is paused if stdin is full */
    if (IS_USED(MODULE_STDIO_RESUME) && (stdio_rx_free() == 0)) {
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
        if (IS_USED(MODULE_STDIO_RESUME)) {
            stdio_set_resume(_resume, NULL);
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
