/*
 * SPDX-FileCopyrightText: 2026 Christopher Büchse
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_epson_escpos
 * @{
 *
 * @file
 * @brief       Dummy transport of the ESC/POS driver writing into a buffer
 *
 * @author      Christopher Büchse <christopher.buechse@tuhh.de>
 *
 * @}
 */

#include <assert.h>
#include <errno.h>
#include <string.h>

#include "epson_escpos.h"

void epson_escpos_dummy_setup(epson_escpos_dummy_t *dummy, uint8_t *buf,
                              size_t size)
{
    assert(dummy);

    memset(dummy, 0, sizeof(*dummy));
    dummy->buf = buf;
    dummy->size = size;
}

static int _open(void *ctx)
{
    (void)ctx;
    return 0;
}

static void _close(void *ctx)
{
    (void)ctx;
}

static int _write(void *ctx, const void *data, size_t len)
{
    epson_escpos_dummy_t *dummy = ctx;

    if (len > dummy->size - dummy->len) {
        return -ENOBUFS;
    }
    memcpy(&dummy->buf[dummy->len], data, len);
    dummy->len += len;
    return 0;
}

static ssize_t _read(void *ctx, void *data, size_t len, uint32_t timeout_ms)
{
    epson_escpos_dummy_t *dummy = ctx;
    (void)timeout_ms;

    if (dummy->rx_len == 0) {
        return -ETIMEDOUT;
    }
    if (len > dummy->rx_len) {
        len = dummy->rx_len;
    }
    memcpy(data, dummy->rx, len);
    dummy->rx += len;
    dummy->rx_len -= len;
    return len;
}

const epson_escpos_transport_t epson_escpos_dummy_transport = {
    .open = _open,
    .close = _close,
    .write = _write,
    .read = _read,
};
