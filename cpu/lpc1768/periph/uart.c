/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_lpc1768
 * @{
 *
 * @file
 * @brief       Implementation of the low-level UART driver for the LPC1768
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @}
 */

#include <stdint.h>
#include <stdio.h>

#include "cpu.h"
#include "periph/uart.h"
#include "periph_conf.h"

/* guard the file in case no UART is defined */
#if UART_0_EN

int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, uart_tx_cb_t tx_cb, void *arg)
{
    /* not yet implemented */
    return -1;
}

int uart_init_blocking(uart_t uart, uint32_t baudrate)
{
    /* not yet implemented */
    return -1;
}

void uart_tx_begin(uart_t uart)
{
    /* not yet implemented */
}

int uart_write(uart_t uart, char data)
{
    /* not yet implemented */
    return -1;
}

int uart_read_blocking(uart_t uart, char *data)
{
    /* not yet implemented */
    return -1;
}

int uart_write_blocking(uart_t uart, char data)
{
    /* not yet implemented */
    return -1;
}

#endif /* UART_0_EN */
