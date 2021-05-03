/*
 * Copyright 2017 Ken Rabold
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_fe310
 * @{
 *
 * @file        timer.c
 * @brief       Low-level timer implementation
 *
 * @author      Ken Rabold
 * @}
 */

#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <inttypes.h>

#include "irq.h"
#include "cpu.h"
#include "periph/uart.h"
#include "plic.h"
#include "vendor/riscv_csr.h"
#include "vendor/platform.h"
#include "vendor/prci_driver.h"

/**
 * @brief   Allocate memory to store the callback functions
 */
static uart_isr_ctx_t isr_ctx[UART_NUMOF];

static inline void _uart_isr(uart_t dev)
{
    uint32_t data = _REG32(uart_config[dev].addr, UART_REG_RXFIFO);

    /* Intr cleared automatically when data is read */
    while ((data & UART_RXFIFO_EMPTY) != (uint32_t)UART_RXFIFO_EMPTY) {
        if (isr_ctx[dev].rx_cb) {
            isr_ctx[dev].rx_cb(isr_ctx[dev].arg, (uint8_t)(data & 0xff));
        }
        data = _REG32(uart_config[dev].addr, UART_REG_RXFIFO);
    }
}

void uart_isr(int num)
{
    switch (num) {
    case INT_UART0_BASE:
        _uart_isr(0);
        break;
    case INT_UART1_BASE:
        _uart_isr(1);
        break;
    default:
        break;
    }
}

static void _drain(uart_t dev)
{
    uint32_t data = _REG32(uart_config[dev].addr, UART_REG_RXFIFO);

    /* Intr cleared automatically when data is read */
    while ((data & UART_RXFIFO_EMPTY) != (uint32_t)UART_RXFIFO_EMPTY) {
        data = _REG32(uart_config[dev].addr, UART_REG_RXFIFO);
    }
}

int uart_init(uart_t dev, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg)
{
    uint32_t uartDiv;

    /* Check for valid UART dev */
    assert(dev < UART_NUMOF);

    /* Save interrupt callback context */
    isr_ctx[dev].rx_cb = rx_cb;
    isr_ctx[dev].arg = arg;

    /* Power on the device */
    uart_poweron(dev);

    /* Calculate baudrate divisor given current CPU clk rate */
    uartDiv = cpu_freq() / baudrate;

    /* Enable UART 8-N-1 at given baudrate */
    _REG32(uart_config[dev].addr, UART_REG_DIV) = uartDiv;

    /* Select IOF0 */
    GPIO_REG(GPIO_IOF_SEL) &= ~(1 << uart_config[dev].tx);
    /* Enable IOF */
    GPIO_REG(GPIO_IOF_EN) |= (1 << uart_config[dev].tx);

    /* Enable TX */
    _REG32(uart_config[dev].addr, UART_REG_TXCTRL) = UART_TXEN;

    /* Enable RX intr if there is a callback */
    if (rx_cb) {
        /* Select IOF0 */
        GPIO_REG(GPIO_IOF_SEL) &= ~(1 << uart_config[dev].rx);
        /* Enable IOF */
        GPIO_REG(GPIO_IOF_EN) |= (1 << uart_config[dev].rx);

        /* Disable ext interrupts when setting up */
        clear_csr(mie, MIP_MEIP);

        /* Configure UART ISR with PLIC */
        plic_set_isr_cb(uart_config[dev].isr_num, uart_isr);
        plic_enable_interrupt(uart_config[dev].isr_num);
        plic_set_priority(uart_config[dev].isr_num, UART_ISR_PRIO);

        /* avoid trap by emptying RX FIFO */
        _drain(dev);

        /* enable RX interrupt */
        _REG32(uart_config[dev].addr, UART_REG_IE) = UART_IP_RXWM;

        /* Enable RX */
        _REG32(uart_config[dev].addr, UART_REG_RXCTRL) = UART_RXEN;

        /* Re-enable ext interrupts */
        set_csr(mie, MIP_MEIP);
    }

    return UART_OK;
}

void uart_write(uart_t dev, const uint8_t *data, size_t len)
{
    for (size_t i = 0; i < len; i++) {
        /* Wait for FIFO to empty */
        while ((_REG32(uart_config[dev].addr,
                       UART_REG_TXFIFO) & UART_TXFIFO_FULL)
               == (uint32_t)UART_TXFIFO_FULL) {}

        /* Write a byte */
        _REG32(uart_config[dev].addr, UART_REG_TXFIFO) = data[i];
    }
}

void uart_poweron(uart_t dev)
{
    (void)dev;
}

void uart_poweroff(uart_t dev)
{
    (void)dev;
}
