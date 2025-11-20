/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include "board.h"
#include "isrpipe.h"
#include "periph/uart.h"

#include "slipdev.h"
#include "slipdev_internal.h"
#include "slipdev_params.h"

#include "stdio_base.h"
#include "stdio_uart.h"

static void _isrpipe_write(void *arg, uint8_t data)
{
    isrpipe_write_one(arg, (char)data);
}

static void _init(void)
{
    /* intentionally overwritten in netdev init so we have stdio before
     * the network device is initialized */
    uart_init(slipdev_params[0].uart, slipdev_params[0].baudrate,
              _isrpipe_write, &stdin_isrpipe);

    slipdev_write_byte(slipdev_params[0].uart, SLIPDEV_END);
}

static ssize_t _write(const void *buffer, size_t len)
{
    mutex_lock(&slipdev_mutex);
    slipdev_write_byte(slipdev_params[0].uart, SLIPDEV_STDIO_START);
    slipdev_write_bytes(slipdev_params[0].uart, buffer, len);
    slipdev_write_byte(slipdev_params[0].uart, SLIPDEV_END);
    mutex_unlock(&slipdev_mutex);
    return len;
}

STDIO_PROVIDER(STDIO_SLIP, _init, NULL, _write)

/** @} */
