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
 */

#include "async_read.h"
#include "kernel_defines.h"
#include "native_internal.h"
#include "isrpipe.h"

#include "stdio_base.h"

#ifndef STDIO_NATIVE_RX_BUFSIZE
#define STDIO_NATIVE_RX_BUFSIZE (64)
#endif

static uint8_t _rx_buf_mem[STDIO_NATIVE_RX_BUFSIZE];
static isrpipe_t stdio_isrpipe = ISRPIPE_INIT(_rx_buf_mem);

static void _async_read_wrapper(int fd, void *arg) {
    uint8_t buf[1];

    int res = real_read(fd, &buf, sizeof(buf));
    if (res > 0) {
        isrpipe_write(arg, buf, res);
    }

    native_async_read_continue(fd);
}

void stdio_init(void)
{
    native_async_read_setup();
    native_async_read_add_int_handler(STDIN_FILENO, &stdio_isrpipe, _async_read_wrapper);
}

ssize_t stdio_read(void* buffer, size_t max_len)
{
    return (ssize_t)isrpipe_read(&stdio_isrpipe, buffer, max_len);
}

ssize_t stdio_write(const void* buffer, size_t len)
{
    return real_write(STDOUT_FILENO, buffer, len);
}
