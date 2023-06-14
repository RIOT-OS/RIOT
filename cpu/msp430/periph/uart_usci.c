/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_msp430_f2xx_g2xx
 * @ingroup     drivers_periph_uart
 * @{
 *
 * @file
 * @brief       Low-level UART driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "cpu.h"
#include "periph_cpu.h"
#include "periph_conf.h"
#include "periph/uart.h"

/**
 * @brief   Keep track of the interrupt context
 * @{
 */
static uart_rx_cb_t ctx_rx_cb;
static void *ctx_isr_arg;
/** @} */

static int init_base(uart_t uart, uint32_t baudrate);

int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg)
{
    if (init_base(uart, baudrate) < 0) {
        return -1;
    }

    /* save interrupt context */
    ctx_rx_cb = rx_cb;
    ctx_isr_arg = arg;
    /* reset interrupt flags and enable RX interrupt */
    UART_IF &= ~(UART_IE_RX_BIT);
    UART_IF |=  (UART_IE_TX_BIT);
    UART_IE |=  (UART_IE_RX_BIT);
    UART_IE &= ~(UART_IE_TX_BIT);
    return 0;
}

static int init_base(uart_t uart, uint32_t baudrate)
{
    if (uart != 0) {
        return -1;
    }

    /* get the default UART for now -> TODO: enable for multiple devices */
    msp_usci_a_t *dev = UART_BASE;

    /* put device in reset mode while configuration is going on */
    dev->CTL1 = UCSWRST;
    /* configure to UART, using SMCLK in 8N1 mode */
    dev->CTL1 |= UCSSEL_SMCLK;
    dev->CTL0 = 0;
    dev->STAT = 0;
    /* configure baudrate */
    uint32_t base = ((msp430_submain_clock_freq() << 7)  / baudrate);
    uint16_t br = (uint16_t)(base >> 7);
    uint8_t brs = (((base & 0x3f) * 8) >> 7);
    dev->BR0 = (uint8_t)br;
    dev->BR1 = (uint8_t)(br >> 8);
    dev->MCTL = (brs << UCBRS_POS);
    /* pin configuration -> TODO: move to GPIO driver once implemented */
    UART_RX_PORT->SEL |= UART_RX_PIN;
    UART_TX_PORT->SEL |= UART_TX_PIN;
    UART_RX_PORT->base.DIR &= ~(UART_RX_PIN);
    UART_TX_PORT->base.DIR |= UART_TX_PIN;
    /* releasing the software reset bit starts the UART */
    dev->CTL1 &= ~(UCSWRST);
    return 0;
}

void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    (void)uart;

    for (size_t i = 0; i < len; i++) {
        while (!(UART_IF & UART_IE_TX_BIT)) {}
        UART_BASE->TXBUF = data[i];
    }
}

void uart_poweron(uart_t uart)
{
    (void)uart;
    /* n/a */
}

void uart_poweroff(uart_t uart)
{
    (void)uart;
    /* n/a */
}

ISR(UART_RX_ISR, isr_uart_0_rx)
{
    __enter_isr();

    uint8_t stat = UART_BASE->STAT;
    uint8_t data = (uint8_t)UART_BASE->RXBUF;

    if (stat & (UCFE | UCOE | UCPE | UCBRK)) {
        /* some error which we do not handle, just do a pseudo read to reset the
         * status register */
        (void)data;
    }
    else {
        ctx_rx_cb(ctx_isr_arg, data);
    }

    __exit_isr();
}
