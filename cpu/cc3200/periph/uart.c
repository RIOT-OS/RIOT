/*
 * Copyright (C) 2015 Rakendra Thapa <rakendrathapa@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_lm4f120
 * @{
 *
 * @file        uart.c
 * @brief       Implementation of the low-level UART driver for the LM4F120
 *
 * @author      Rakendra Thapa <rakendrathapa@gmail.com>
 *
 * @}
 */

#include <stdint.h>

#include "assert.h"
#include "cpu.h"
#include "sched.h"
#include "thread.h"
#include "periph/uart.h"
#include "periph_conf.h"

/**
 * @brief UART device configurations
 */
// static uart_isr_ctx_t config[UART_NUMOF];

/**
 * The list of UART peripherals.
 */
// static const unsigned long g_ulUARTPeriph[3] =
// {
// };

/**
 * The list of all possible base address of the console UART
 */
// static const unsigned long g_ulUARTBase[3] =
// {
// };

/**
 * The list of possible interrupts for the console UART.
 */
// static const unsigned long g_ulUARTInt[3] =
// {
// };

// static int init_base(uart_t uart, uint32_t baudrate);

/**
 * Configuring the UART console
 */
int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg)
{
    return 0;
}

// static int init_base(uart_t uart, uint32_t baudrate)
// {
//     return 0;
// }

void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
}

void uart_poweron(uart_t uart)
{
}

void uart_poweroff(uart_t uart)
{
}

/**
 * The UART interrupt handler.
 */
void isr_uart0(void)
{
}
