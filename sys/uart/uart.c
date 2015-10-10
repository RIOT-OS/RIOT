/*
 * Copyright (C) 2015 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    driver_periph_extended_uart UART
 * @ingroup     driver_periph
 * @brief       Low-level extended UART peripheral driver
 *
 * @{
 *
 * @file
 * @brief       Low-level UART driver implementation
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 *
 * @}
 */

#include <stdint.h>
#include <stdio.h>

#include "periph/uart.h"


static uartdev_ops_t *uartdev_lt[UART_NUMOF];

int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg)
{
    if (uartdev_lt[uart]) {
       return uartdev_lt[uart]->uart_init(uart, baudrate, rx_cb, arg);
    }
    return -1;
}

void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    if (uartdev_lt[uart]) {
        uartdev_lt[uart]->uart_write(uart, data, len);
    }
}

void uart_poweron(uart_t uart)
{
    if (uartdev_lt[uart]) {
        uartdev_lt[uart]->uart_poweron(uart);
    }
}


void uart_poweroff(uart_t uart)
{
    if (uartdev_lt[uart]) {
        uartdev_lt[uart]->uart_poweroff(uart);
    }
}


int uartdev_register_driver(uartdev_ops_t *uart_ops)
{
    if (uart_ops->dev > UART_NUMOF) {
        return -1;
    }

    if (uartdev_lt[uart_ops->dev] == NULL) {
        uartdev_lt[uart_ops->dev] = uart_ops;
        return 0;
    }
    return -1;
}
