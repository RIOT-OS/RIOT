/*
 * SPDX-FileCopyrightText: 2026 Christopher Büchse
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_epson_escpos
 * @{
 *
 * @file
 * @brief       UART transport of the ESC/POS driver
 *
 * @author      Christopher Büchse <christopher.buechse@tuhh.de>
 *
 * @}
 */

#include <assert.h>
#include <errno.h>

#include "epson_escpos.h"
#include "isrpipe/read_timeout.h"
#include "time_units.h"
#include "ztimer.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/* number of bytes written between checks of the BUSY line */
#define BUSY_CHUNK_SIZE     (8U)

int epson_escpos_uart_setup(epson_escpos_uart_t *uart,
                            const epson_escpos_uart_params_t *params)
{
    assert(uart && params);

    uart->params = params;
    isrpipe_init(&uart->rx, uart->rx_buf, sizeof(uart->rx_buf));
    return 0;
}

static void _rx_cb(void *arg, uint8_t data)
{
    epson_escpos_uart_t *uart = arg;
    isrpipe_write_one(&uart->rx, data);
}

static int _open(void *ctx)
{
    epson_escpos_uart_t *uart = ctx;
    const epson_escpos_uart_params_t *p = uart->params;

    if (gpio_is_valid(p->busy_pin)) {
        int res = gpio_init(p->busy_pin, GPIO_IN);
        if (res < 0) {
            return res;
        }
    }

    isrpipe_clear(&uart->rx);
    int res = uart_init(p->uart, p->baudrate, _rx_cb, uart);
    if (res != UART_OK) {
        DEBUG("[epson_escpos_uart] uart_init failed: %d\n", res);
        return -ENODEV;
    }
    return 0;
}

static void _close(void *ctx)
{
    epson_escpos_uart_t *uart = ctx;
    uart_poweroff(uart->params->uart);
}

static void _wait_ready(const epson_escpos_uart_params_t *p)
{
    if (!gpio_is_valid(p->busy_pin)) {
        return;
    }
    while (!!gpio_read(p->busy_pin) == p->busy_level) {
        ztimer_sleep(ZTIMER_MSEC, 1);
    }
}

static int _write(void *ctx, const void *data, size_t len)
{
    epson_escpos_uart_t *uart = ctx;
    const epson_escpos_uart_params_t *p = uart->params;
    const uint8_t *pos = data;

    while (len) {
        size_t chunk = gpio_is_valid(p->busy_pin) && (len > BUSY_CHUNK_SIZE)
                     ? BUSY_CHUNK_SIZE : len;
        _wait_ready(p);
        uart_write(p->uart, pos, chunk);
        pos += chunk;
        len -= chunk;
    }
    return 0;
}

static ssize_t _read(void *ctx, void *data, size_t len, uint32_t timeout_ms)
{
    epson_escpos_uart_t *uart = ctx;
    return isrpipe_read_timeout(&uart->rx, data, len, timeout_ms * US_PER_MS);
}

const epson_escpos_transport_t epson_escpos_uart_transport = {
    .open = _open,
    .close = _close,
    .write = _write,
    .read = _read,
};
