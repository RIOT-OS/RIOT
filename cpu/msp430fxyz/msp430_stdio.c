/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_msp430fxyz
 * @{
 *
 * @file
 * @brief       Implementation of getchar and putchar for MSP430 CPUs
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "irq.h"
#include "mutex.h"
#include "board.h"
#include "ringbuffer.h"
#include "periph/uart.h"

#include <stdio.h>

static mutex_t rx_lock = MUTEX_INIT;
static ringbuffer_t rx_buf;
static char rx_buf_mem[STDIO_RX_BUFSIZE];

static inline int safe_read(void)
{
    int res;
    unsigned state = disableIRQ();
    res = ringbuffer_get_one(&rx_buf);
    restoreIRQ(state);
    return res;
}

static void rx_cb(void *arg, char data)
{
    (void)arg;

    ringbuffer_add_one(&rx_buf, data);
    /* this is a little dirty hack: it seems the MSP430 boards are too slow for
     * processing data @ 115200 baud and calling mutex_unock() for each byte. By
     * asserting, that the STDIO is used for the shell and we are only
     * interested in completed lines anyway, we can reduce the overhead here by
     * only waking the shell on newline chars */
    if (data == '\n') {
        mutex_unlock(&rx_lock);
    }
}

void msp430_stdio_init(void)
{
    mutex_lock(&rx_lock);
    ringbuffer_init(&rx_buf, rx_buf_mem, STDIO_RX_BUFSIZE);
    uart_init(STDIO, STDIO_BAUDRATE, rx_cb, NULL, 0);
}

/**
 * @brief   Get one character from STDIO - used by the libc
 */
int getchar(void)
{
    int res = safe_read();

    while (res == -1) {
        mutex_lock(&rx_lock);
        res = safe_read();
    }
    return res;
}

/**
 * @brief   Write one character to the STDIO UART interface - used by e.g.
 *          printf and puts
 */
int putchar(int c)
{
    uart_write_blocking(STDIO, (char)c);
    return 1;
}
