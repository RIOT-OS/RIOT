/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @file
 * @author Martine S. Lenders <m.lenders@fu-berlin.de>
 * @author Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#include "async_read.h"
#include "kernel_defines.h"
#include "native_internal.h"

#include "stdio_base.h"

static void _async_read_wrapper(int fd, void *arg) {
    uint8_t buf[STDIO_RX_BUFSIZE];

    int res = real_read(fd, &buf, sizeof(buf));
    if (res > 0) {
        isrpipe_write(arg, buf, res);
    }

    native_async_read_continue(fd);
}

static void _init(void)
{
    native_async_read_setup();
    if (IS_USED(MODULE_STDIN)) {
        native_async_read_add_int_handler(STDIN_FILENO, &stdin_isrpipe,
                                          _async_read_wrapper);
    }
}

static ssize_t _write(const void* buffer, size_t len)
{
    return real_write(STDOUT_FILENO, buffer, len);
}

STDIO_PROVIDER(STDIO_NATIVE, _init, NULL, _write)

/** @} */
