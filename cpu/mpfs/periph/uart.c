/*
 * Copyright (C) 2021 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_mpfs
 * @ingroup     drivers_periph_uart
 * @{
 *
 * @file
 * @brief       Low-level UART driver implementation
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 *
 * @}
 */

#include "cpu.h"
#include "board.h"
#include "periph_cpu.h"
#include "periph/gpio.h"
#include "periph/uart.h"
#include "plic.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/**
 * @brief   Allocate memory to store the callback functions & buffers
 */
static uart_isr_ctx_t uart_ctx[UART_NUMOF];

/**
 * @brief   Get the pointer to the base register of the given UART device
 *
 * @param[in] dev       UART device identifier
 *
 * @return              base register address
 */
static inline mss_uart_instance_t *dev(uart_t dev)
{
    return uart_config[dev].dev;
}

static inline MSS_UART_TypeDef *base_addr(uart_t dev)
{
    return uart_config[dev].base_addr;
}

static void _uart_isr(mss_uart_instance_t* this_uart, unsigned uartn)
{
    uint8_t byte;
    MSS_UART_get_rx(this_uart, &byte, 1);
    uart_ctx[uartn].rx_cb(uart_ctx[uartn].arg, byte);
}

void uart_isr(int num)
{
    switch (num) {
        case MMUART0_PLIC_77:
            _uart_isr(&g_mss_uart0_lo, 0);
            break;
        case MMUART1_PLIC:
            _uart_isr(&g_mss_uart1_lo, 1);
            break;
        case MMUART2_PLIC:
            _uart_isr(&g_mss_uart2_lo, 2);
            break;
        case MMUART3_PLIC:
            _uart_isr(&g_mss_uart3_lo, 3);
            break;
        case MMUART4_PLIC:
            _uart_isr(&g_mss_uart4_lo, 4);
            break;
        default:
            assert(0);
            break;
    }
}

int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg)
{
    mss_config_clk_rst(uart_config[uart].clk, MPFS_HAL_FIRST_HART, PERIPHERAL_ON);

    MSS_UART_init(dev(uart), baudrate,
                  MSS_UART_DATA_8_BITS | MSS_UART_NO_PARITY | MSS_UART_ONE_STOP_BIT);

    if (rx_cb) {
        uart_ctx[uart].rx_cb = rx_cb;
        uart_ctx[uart].arg = arg;
        plic_set_isr_cb(uart_config[uart].irqn, uart_isr);
        plic_set_priority(uart_config[uart].irqn, 2);

        /* Set IRQ trigger level, write directly as register is Write Only */
        base_addr(uart)->FCR = MSS_UART_FIFO_SINGLE_BYTE;
        /* Enable Receive data interrupt */
        base_addr(uart)->IER = MSS_UART_RBF_IRQ;
        /* Enable PLIC irq */
        plic_enable_interrupt(uart_config[uart].irqn);
    }
    return UART_OK;
}

void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    MSS_UART_polled_tx(dev(uart), data, len);
}

void uart_poweron(uart_t uart)
{
    (void)uart;
}

void uart_poweroff(uart_t uart)
{
    (void)uart;
}
