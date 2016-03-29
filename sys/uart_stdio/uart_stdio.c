/*
 * Copyright (C) 2013 INRIA
 *               2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup sys
 * @{
 *
 * @file
 * @brief UART stdio implementation
 *
 * This file implements a UART callback and read/write functions.
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdio.h>
#include "uart_stdio.h"

#include "tsrb.h"
#include "thread.h"
#include "mutex.h"
#include "irq.h"

#include "board.h"
#include "periph/uart.h"

#ifdef USE_ETHOS_FOR_STDIO
#include "ethos.h"
extern ethos_t ethos;
#endif

#define ENABLE_DEBUG 0
#include "debug.h"

/**
 * @brief use mutex for waiting on incoming UART chars
 */
static mutex_t _rx_mutex = MUTEX_INIT;
static char _rx_buf_mem[UART_STDIO_RX_BUFSIZE];
static tsrb_t _rx_buf = TSRB_INIT(_rx_buf_mem);

/**
 * @brief Receive a new character from the UART and put it into the receive buffer
 */
void uart_stdio_rx_cb(void *arg, uint8_t data)
{
    (void)arg;
    tsrb_add_one(&_rx_buf, (uint8_t)data);
    mutex_unlock(&_rx_mutex);
}

void uart_stdio_init(void)
{
#ifndef USE_ETHOS_FOR_STDIO
    uart_init(UART_STDIO_DEV, UART_STDIO_BAUDRATE, uart_stdio_rx_cb, NULL);
#else
    uart_init(ETHOS_UART, ETHOS_BAUDRATE, uart_stdio_rx_cb, NULL);
#endif
}

int uart_stdio_read(char* buffer, int count)
{
    int res;
    while (!(res = tsrb_get(&_rx_buf, buffer, count))) {
        mutex_lock(&_rx_mutex);
    }
    return res;
}

int uart_stdio_write(const char* buffer, int len)
{
#ifndef USE_ETHOS_FOR_STDIO
    uart_write(UART_STDIO_DEV, (uint8_t *)buffer, (size_t)len);
#else
    ethos_send_frame(&ethos, (uint8_t*)buffer, len, ETHOS_FRAME_TYPE_TEXT);
#endif
    return len;
}
