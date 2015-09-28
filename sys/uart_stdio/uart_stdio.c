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

#include "cpu_conf.h"
#include "tsrb.h"
#include "thread.h"
#include "mutex.h"
#include "irq.h"

#include "periph/uart.h"

#include "board.h"
#include "periph/uart.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#ifndef STDIO
#define STDIO               (0)
#endif

#ifndef STDIO_BAUDRATE
#define STDIO_BAUDRATE      (115200)
#endif

#ifndef STDIO_RX_BUFSIZE
#define STDIO_RX_BUFSIZE    (64)
#endif

/**
 * @brief use mutex for waiting on incoming UART chars
 */
static mutex_t _rx_mutex = MUTEX_INIT;
static char _rx_buf_mem[STDIO_RX_BUFSIZE];
static tsrb_t _rx_buf = TSRB_INIT(_rx_buf_mem);

/**
 * @brief Receive a new character from the UART and put it into the receive buffer
 */
void uart_stdio_rx_cb(void *arg, char data)
{
    (void)arg;
    tsrb_add_one(&_rx_buf, data);
    mutex_unlock(&_rx_mutex);
}

void uart_stdio_init(void)
{
    mutex_lock(&_rx_mutex);
    uart_init(STDIO, STDIO_BAUDRATE, uart_stdio_rx_cb, 0, 0);
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
    unsigned int i = len;

    while (i--) {
        uart_write_blocking(STDIO, *buffer++);
    }

    return len;
}
