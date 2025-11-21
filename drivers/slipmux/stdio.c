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

#include "slipmux_internal.h"
#include "slipmux_params.h"

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
    uart_init(slipmux_params[0].uart, slipmux_params[0].baudrate,
              _isrpipe_write, &stdin_isrpipe);

    slipmux_write_byte(slipmux_params[0].uart, SLIPMUX_END);
}

static ssize_t _write(const void *buffer, size_t len)
{
    slipmux_lock();
    slipmux_write_byte(slipmux_params[0].uart, SLIPMUX_STDIO_START);
    slipmux_write_bytes(slipmux_params[0].uart, buffer, len);
    slipmux_write_byte(slipmux_params[0].uart, SLIPMUX_END);
    slipmux_unlock();
    return len;
}

STDIO_PROVIDER(STDIO_SLIPMUX, _init, NULL, _write)

/** @} */
