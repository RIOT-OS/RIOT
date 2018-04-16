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

#include "irq.h"
#include "cpu.h"
#include "periph_cpu.h"
#include "periph_conf.h"
#include "periph/uart.h"
#include "vendor/encoding.h"
#include "vendor/platform.h"
#include "vendor/plic_driver.h"
#include "vendor/prci_driver.h"

/**
 * @brief   Allocate memory to store the callback functions
 */
static uart_isr_ctx_t isr_ctx[UART_NUMOF];

void uart_isr(int num)
{
    uint32_t data;

    /* Invoke callback function */
    if (num == INT_UART0_BASE) {
        data = UART0_REG(UART_REG_RXFIFO);

        /* Intr cleared automatically when data is read */
        while ((data & UART_RXFIFO_EMPTY) != (uint32_t)UART_RXFIFO_EMPTY) {
            isr_ctx[0].rx_cb(isr_ctx[0].arg, (uint8_t) data);
            data = UART0_REG(UART_REG_RXFIFO);
        }
    }

    if (num == INT_UART1_BASE) {
        data = UART1_REG(UART_REG_RXFIFO);

        /* Intr cleared automatically when data is read */
        while ((data & UART_RXFIFO_EMPTY) != (uint32_t)UART_RXFIFO_EMPTY) {
            isr_ctx[1].rx_cb(isr_ctx[1].arg, (uint8_t) data);
            data = UART1_REG(UART_REG_RXFIFO);
        }
    }
}

int uart_init(uart_t dev, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg)
{
    uint32_t uartDiv;

    /* Check for valid UART dev */
    if (dev >= UART_NUMOF) {
        return UART_NODEV;
    }

    /* Save interrupt callback context */
    isr_ctx[dev].rx_cb = rx_cb;
    isr_ctx[dev].arg = arg;

    /* Power on the device */
    uart_poweron(dev);

    /* Calculate buadrate divisor given current CPU clk rate
     * Ignore the first run (icache needs to be warm) */
    uartDiv = PRCI_measure_mcycle_freq(1000, RTC_FREQ);
    /* cppcheck-suppress redundantAssignment */
    uartDiv = PRCI_measure_mcycle_freq(1000, RTC_FREQ);
    uartDiv = uartDiv / baudrate;

    /* Enable UART 8-N-1 at given baudrate */
    if (dev == 0) {
        /* Config UART */
        UART0_REG(UART_REG_DIV) = uartDiv;
        UART0_REG(UART_REG_TXCTRL) = UART_TXEN;

        /* Enable RX intr if there is a callback */
        if (rx_cb != NULL) {
            /* Disable ext interrupts when setting up */
            clear_csr(mie, MIP_MEIP);

            /* Configure UART ISR with PLIC */
            set_external_isr_cb(INT_UART0_BASE, uart_isr);
            PLIC_enable_interrupt(INT_UART0_BASE);
            PLIC_set_priority(INT_UART0_BASE, UART0_RX_INTR_PRIORITY);
            UART0_REG(UART_REG_IE) = UART_IP_RXWM;
            UART0_REG(UART_REG_RXCTRL) = UART_RXEN;

            /* Re-eanble ext interrupts */
            set_csr(mie, MIP_MEIP);
        }
    }

    if (dev == 1) {
        /* Config UART */
        UART1_REG(UART_REG_DIV) = uartDiv;
        UART1_REG(UART_REG_TXCTRL) = UART_TXEN;

        /* Enable RX intr if there is a callback */
        if (rx_cb != NULL) {
            /* Disable ext interrupts when setting up */
            clear_csr(mie, MIP_MEIP);

            /* Configure UART ISR with PLIC */
            set_external_isr_cb(INT_UART1_BASE, uart_isr);
            PLIC_enable_interrupt(INT_UART1_BASE);
            PLIC_set_priority(INT_UART1_BASE, UART1_RX_INTR_PRIORITY);
            UART1_REG(UART_REG_IE) = UART_IP_RXWM;
            UART1_REG(UART_REG_RXCTRL) = UART_RXEN;

            /* Re-eanble ext interrupts */
            set_csr(mie, MIP_MEIP);
        }
    }

    return UART_OK;
}

void uart_write(uart_t dev, const uint8_t *data, size_t len)
{
    if (dev == 0) {
        for (size_t i = 0; i < len; i++) {
            /* Wait for FIFO to empty */
            while ((UART0_REG(UART_REG_TXFIFO) & UART_TXFIFO_FULL)
                   == (uint32_t)UART_TXFIFO_FULL)
                ;

            /* Write a byte */
            UART0_REG(UART_REG_TXFIFO) = data[i];
        }
    }

    if (dev == 1) {
        for (size_t i = 0; i < len; i++) {
            /* Wait for FIFO to empty */
            while ((UART1_REG(UART_REG_TXFIFO) & UART_TXFIFO_FULL)
                   == (uint32_t)UART_TXFIFO_FULL)
                ;

            /* Write a byte */
            UART1_REG(UART_REG_TXFIFO) = data[i];
        }
    }
}

void uart_poweron(uart_t dev)
{
    (void) dev;
}

void uart_poweroff(uart_t dev)
{
    (void) dev;
}
