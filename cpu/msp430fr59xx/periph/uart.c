/*
 * Copyright (C) 2018 INRIA
 *               2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_msp430fr59xx
 * @ingroup     drivers_periph_uart
 * @{
 *
 * @file
 * @brief       Low-level UART driver implementation
 *
 * @author      Loïc Saos <Loic.Saos@insa-lyon.fr>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "cpu.h"
#include "periph_cpu.h"
#include "periph_conf.h"
#include "periph/uart.h"
#include "periph/gpio.h"

/**
 * @brief   Keep track of the interrupt context
 * @{
 */
static uart_rx_cb_t ctx_rx_cb;
static void *ctx_isr_arg;
/** @} */

static int init_base(uart_t uart, uint32_t baudrate);

#ifdef UART_USE_USCI

int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg)
{
    int res = init_base(uart, baudrate);

    if (res != UART_OK) {
        return res;
    }

    /* save interrupt context */
    ctx_rx_cb = rx_cb;
    ctx_isr_arg = arg;

    /* reset interrupt flags and enable RX interrupt */
    UART_IF &= ~UART_IE_RX_BIT;
    UART_IF |=  UART_IE_TX_BIT;
    UART_IE |=  UART_IE_RX_BIT;
    UART_IE &= ~UART_IE_TX_BIT;

    return UART_OK;
}

static int init_base(uart_t uart, uint32_t baudrate)
{
    if (uart != 0) {
        return UART_NODEV;
    }

    /* compute configuration values */
    const uint32_t divInt = CLOCK_CMCLK / baudrate;
    const uint32_t divFracNumerator = CLOCK_CMCLK - (divInt * baudrate);
    const uint32_t divFracNumX8 = divFracNumerator * 8;
    const uint32_t divFracX8 = divFracNumX8 / baudrate;

    unsigned int brsVal;

    if (((divFracNumX8 - (divFracX8 * baudrate)) * 10) / baudrate < 5) {
        brsVal = divFracX8 << 1;
    }
    else {
        brsVal = (divFracX8 + 1) << 1;
    }

    /* get the default UART for now -> TODO: enable for multiple devices */
    msp_usci_t *dev = UART_BASE;

    // Configure pins USCI_A0 UART operation
    gpio_init(UART_RX_PIN, GPIO_IN);
    gpio_init(UART_TX_PIN, GPIO_OUT);

    gpio_periph_mode(UART_RX_PIN, false, true, false);
    gpio_periph_mode(UART_TX_PIN, false, true, false);

    dev->CTLW0 = USCI_CTL0_SWRST;

    dev->CTLW0 |= USCI_CTL0_SSEL_SMCLK;
    dev->BR0 = divInt & 0x00ff;
    dev->BR1 = (divInt >> 8) & 0xff;
    dev->MCTLW |= brsVal;

    dev->CTLW0 &= ~USCI_CTL0_SWRST;

    return UART_OK;
}

void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    (void)uart;

    for (size_t i = 0; i < len; i++) {
        while (!(UART_IF & UART_IFG_TX_BIT)) {}
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

    switch (UART_BASE->IV) {
        case USCI_IV_RXIFG:
            ctx_rx_cb(ctx_isr_arg, (uint8_t)UART_BASE->RXBUF);
            break;
        default:
            break;
    }

    UART_IF &= ~UART_IE_RX_BIT;

    __exit_isr();
}

#endif // UART_USE_USCI
