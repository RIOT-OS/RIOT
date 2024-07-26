/*
 * Copyright (C) 2013 INRIA
 *               2015 Kaspar Schleiser <kaspar@schleiser.de>
 *               2016 Eistec AB
 *               2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys
 * @{
 *
 * @file
 * @brief       STDIO over UART implementation
 *
 * This file implements a UART callback and the STDIO read/write functions
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <errno.h>
#include <string.h>

#include "board.h"
#include "kernel_defines.h"
#include "periph/uart.h"
#include "stdio_base.h"
#include "stdio_uart.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static void _isrpipe_write_one_wrapper(void *arg, uint8_t value)
{
    isrpipe_write_one(arg, value);
}

static void _init(void)
{
    uart_rx_cb_t cb = NULL;
    void *arg = NULL;

    if (IS_USED(MODULE_STDIO_UART_RX)) {
        cb = _isrpipe_write_one_wrapper;
        arg = &stdin_isrpipe;
    }

    uart_init(STDIO_UART_DEV, STDIO_UART_BAUDRATE, cb, arg);
}

#if IS_USED(MODULE_STDIO_FLUSH_RX)
void stdio_flush_rx(void)
{
    tsrb_clear(&stdin_isrpipe.tsrb);
}
#endif

static ssize_t _write(const void *buffer, size_t len)
{
    ssize_t result = len;
    if (IS_USED(MODULE_STDIO_UART_ONLCR)) {
        static const uint8_t crlf[2] = { (uint8_t)'\r', (uint8_t)'\n' };
        const uint8_t *buf = buffer;
        while (len) {
            const uint8_t *pos = memchr(buf, '\n', len);
            size_t chunk_len = (pos != NULL)
                             ? (uintptr_t)pos - (uintptr_t)buf
                             : len;
            uart_write(STDIO_UART_DEV, buf, chunk_len);
            buf += chunk_len;
            len -= chunk_len;
            if (len) {
                uart_write(STDIO_UART_DEV, crlf, sizeof(crlf));
                buf++;
                len--;
            }
        }
    }
    else {
        uart_write(STDIO_UART_DEV, (const uint8_t *)buffer, len);
    }
    return result;
}

static void _detach(void)
{
    uart_poweroff(STDIO_UART_DEV);
}

STDIO_PROVIDER(STDIO_UART, _init, _detach, _write)
