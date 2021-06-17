/*
 * Copyright (C) 2021 Ishraq Ibne Ashraf <ishraq.i.ashraf@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_rp2040
 * @ingroup     drivers_periph_uart
 * @{
 *
 * @file        uart.c
 * @brief       Implementation of the low-level UART driver for the rp2040
 *
 * @author      Ishraq Ibne Ashraf <ishraq.i.ashraf@gmail.com>
 *
 * @}
 */

#include <stdint.h>

#include "periph_cpu.h"
#include "periph_conf.h"
#include "periph/uart.h"

/**
 * @brief UART device configurations
 */
//static uart_isr_ctx_t config[NUM_UART];

/**
 * Configuring the UART console
 */
int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg)
{
    // Release UART HW reset.
    (void)(uart);
    (void)(baudrate);
    (void)(rx_cb);
    (void)(arg);

    return UART_OK;
}

void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    (void)(uart);
    (void)(data);
    (void)(len);
}

/**
 * The UART interrupt handler.
 */

void isr_uart(int dev) {
    (void)(dev);
}

void isr_uart0(void) {
    isr_uart(0);

    cortexm_isr_end();
}
void isr_uart1(void) {
    isr_uart(1);

    cortexm_isr_end();
}
