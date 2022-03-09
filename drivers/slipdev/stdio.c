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

static uint8_t _rx_buf_mem[STDIO_UART_RX_BUFSIZE];

isrpipe_t slipdev_stdio_isrpipe = ISRPIPE_INIT(_rx_buf_mem);
mutex_t slipdev_mutex = MUTEX_INIT;

static void _isrpipe_write(void *arg, uint8_t data)
{
    isrpipe_write_one(arg, (char)data);
}

void stdio_init(void)
{
    /* intentionally overwritten in netdev init so we have stdio before
     * the network device is initialized is initialized */
    uart_init(slipdev_params[0].uart, slipdev_params[0].baudrate,
              (uart_rx_cb_t)_isrpipe_write, &slipdev_stdio_isrpipe);
}

ssize_t stdio_read(void *buffer, size_t len)
{
    uint8_t *ptr = buffer;
    bool escaped = false;
    uint8_t byte;

    do {
        int read = isrpipe_read(&slipdev_stdio_isrpipe, &byte, 1);
        int tmp;

        if (read == 0) {
            continue;
        }
        else if (len == 0) {
            return -ENOBUFS;
        }
        tmp = slipdev_unstuff_readbyte(ptr, byte, &escaped);
        ptr += tmp;
        if ((unsigned)(ptr - (uint8_t *)buffer) > len) {
            while (byte != SLIPDEV_END) {
                /* clear out unreceived packet */
                isrpipe_read(&slipdev_stdio_isrpipe, &byte, 1);
            }
            return -ENOBUFS;
        }
    } while (byte != SLIPDEV_END);
    return ptr - (uint8_t *)buffer;
}

ssize_t stdio_write(const void *buffer, size_t len)
{
    mutex_lock(&slipdev_mutex);
    slipdev_write_byte(slipdev_params[0].uart, SLIPDEV_STDIO_START);
    slipdev_write_bytes(slipdev_params[0].uart, buffer, len);
    slipdev_write_byte(slipdev_params[0].uart, SLIPDEV_END);
    mutex_unlock(&slipdev_mutex);
    return len;
}

/** @} */
