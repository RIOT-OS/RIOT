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
    /* intentionally overwritten in slipmux init so we have stdio before
     * the network device is initialized */
    uart_init(slipmux_params[0].uart, slipmux_params[0].baudrate,
              _isrpipe_write, &stdin_isrpipe);

    slipmux_write_byte(slipmux_params[0].uart, SLIPMUX_END);
}

static ssize_t _write(const void *buffer, size_t len)
{
    slipmux_lock(&slipmux_devs[0]);
    slipmux_write_byte(slipmux_params[0].uart, SLIPMUX_STDIO_START);
    slipmux_write_bytes(slipmux_params[0].uart, buffer, len);
    slipmux_write_byte(slipmux_params[0].uart, SLIPMUX_END);
    slipmux_unlock(&slipmux_devs[0]);
    return len;
}

STDIO_PROVIDER(STDIO_SLIPMUX, _init, NULL, _write)

/** @} */
