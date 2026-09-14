/*
 * SPDX-FileCopyrightText: 2018 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include "board.h"
#include "periph/uart.h"

#include "slipdev.h"
#include "slipdev_internal.h"
#include "slipdev_params.h"

#include "stdio_base.h"
#include "stdio_uart.h"

static inline void _slipdev_lock(void)
{
    if (IS_USED(MODULE_SLIPDEV_CONFIG) || IS_USED(MODULE_SLIPDEV_NET)) {
        mutex_lock(&slipdev_mutex);
    }
}

static inline void _slipdev_unlock(void)
{
    if (IS_USED(MODULE_SLIPDEV_CONFIG) || IS_USED(MODULE_SLIPDEV_NET)) {
        mutex_unlock(&slipdev_mutex);
    }
}

static void _stdio_rx_write(void *arg, uint8_t data)
{
    (void)arg;
    stdio_rx_write_one(data);
}

static void _init(void)
{
    /* intentionally overwritten in netdev init so we have stdio before
     * the network device is initialized */
    uart_init(slipdev_params[0].uart, slipdev_params[0].baudrate,
              _stdio_rx_write, NULL);

    slipdev_write_byte(slipdev_params[0].uart, SLIPDEV_END);
}

static ssize_t _write(const void *buffer, size_t len)
{
    _slipdev_lock();
    slipdev_write_byte(slipdev_params[0].uart, SLIPDEV_START_STDIO);
    slipdev_write_bytes(slipdev_params[0].uart, buffer, len);
    slipdev_write_byte(slipdev_params[0].uart, SLIPDEV_END);
    _slipdev_unlock();
    return len;
}

STDIO_PROVIDER(STDIO_SLIP, _init, NULL, _write)

/** @} */
