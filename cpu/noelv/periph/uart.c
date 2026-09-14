/*
 * SPDX-FileCopyrightText: 2026 Matvii Ivashchenko
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_noelv
 * @{
 *
 * @file
 * @brief       Low-level UART driver for GRLIB APBUART
 *
 * @author      Matvii Ivashchenko
 * @}
 */

#include "assert.h"

#include "board.h"
#include "cpu.h"
#include "periph/uart.h"
#include "periph_conf.h"
#include "plic.h"
#include "vendor/apbuart.h"
#include "vendor/riscv_csr.h"

/**
 * @brief   Allocate memory to store the callback functions
 */
static uart_isr_ctx_t isr_ctx[UART_NUMOF];

static inline void _uart_isr(uart_t dev)
{
    /* Read all available bytes from RX FIFO */
    while (APBUART_REG(uart_config[dev].addr, APBUART_STATUS) & APBUART_STATUS_DR) {
        uint8_t data = APBUART_REG(uart_config[dev].addr, APBUART_DATA) & 0xFF;
        if (isr_ctx[dev].rx_cb) {
            isr_ctx[dev].rx_cb(isr_ctx[dev].arg, data);
        }
    }
}

static void uart_isr(int irq)
{
    /* Find which UART triggered this IRQ and dispatch */
    for (uart_t dev = 0; dev < UART_NUMOF; dev++) {
        if ((int)uart_config[dev].irq == irq) {
            _uart_isr(dev);
            return;
        }
    }
}

static void _drain(uart_t dev)
{
    /* Discard all bytes currently in RX FIFO */
    while (APBUART_REG(uart_config[dev].addr, APBUART_STATUS) & APBUART_STATUS_DR) {
        (void)APBUART_REG(uart_config[dev].addr, APBUART_DATA);
    }
}

int uart_init(uart_t dev, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg)
{
    assert(dev < UART_NUMOF);

    /* Save callback */
    isr_ctx[dev].rx_cb = rx_cb;
    isr_ctx[dev].arg   = arg;

    uart_poweron(dev);

    /* Set baudrate: SCALER = SYSCLK / (baudrate * 8) - 1 */
    APBUART_REG(uart_config[dev].addr, APBUART_SCALER) =
        APBUART_SCALER_VAL(CLOCK_CORECLOCK, baudrate);

    if (rx_cb) {
        /* Disable external interrupts while configuring PLIC */
        clear_csr(mie, MIP_MEIP);

        /* Register ISR callback with PLIC */
        plic_set_isr_cb(uart_config[dev].irq, uart_isr);
        plic_enable_interrupt(uart_config[dev].irq);
        plic_set_priority(uart_config[dev].irq, UART_ISR_PRIO);

        /* Drain RX FIFO to avoid spurious interrupt on enable */
        _drain(dev);

        /* Enable TX + RX + RX interrupt */
        APBUART_REG(uart_config[dev].addr, APBUART_CTRL) =
            APBUART_CTRL_TE | APBUART_CTRL_RE | APBUART_CTRL_RI;

        /* Re-enable external interrupts */
        set_csr(mie, MIP_MEIP);
    }
    else {
        /* TX only mode — no interrupt needed */
        APBUART_REG(uart_config[dev].addr, APBUART_CTRL) =
            APBUART_CTRL_TE | APBUART_CTRL_RE;
    }

    return UART_OK;
}

void uart_write(uart_t dev, const uint8_t *data, size_t len)
{
    for (size_t i = 0; i < len; i++) {
        /* Wait until TX FIFO has space */
        while (APBUART_REG(uart_config[dev].addr, APBUART_STATUS) & APBUART_STATUS_TF) {}

        APBUART_REG(uart_config[dev].addr, APBUART_DATA) = data[i];
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
