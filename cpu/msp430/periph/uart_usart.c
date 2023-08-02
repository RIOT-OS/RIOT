/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_msp430_x1xx
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
    int res = init_base(uart, baudrate);
    if (res != UART_OK) {
        return res;
    }

    /* save interrupt context */
    ctx_rx_cb = rx_cb;
    ctx_isr_arg = arg;
    /* reset interrupt flags and enable RX interrupt */
    UART_SFR->IE &= ~(UART_IE_TX_BIT);
    UART_SFR->IFG &= ~(UART_IE_RX_BIT);
    UART_SFR->IFG |=  (UART_IE_TX_BIT);
    UART_SFR->IE |=  (UART_IE_RX_BIT);
    return UART_OK;
}

static int init_base(uart_t uart, uint32_t baudrate)
{
    if (uart != 0) {
        return UART_NODEV;
    }

    /* get the default UART for now -> TODO: enable for multiple devices */
    msp_usart_t *dev = UART_BASE;

    /* power off and reset device */
    uart_poweroff(uart);
    dev->CTL = SWRST;
    /* configure to 8N1 and using the SMCLK*/
    dev->CTL |= CHAR;
    dev->TCTL = (TXEPT | UXTCTL_SSEL_SMCLK);
    dev->RCTL = 0x00;
    /* baudrate configuration */
    uint16_t br = (uint16_t)(msp430_submain_clock_freq() / baudrate);
    dev->BR0 = (uint8_t)br;
    dev->BR1 = (uint8_t)(br >> 8);
    /* TODO: calculate value for modulation register */
    dev->MCTL = 0;
    /* configure pins -> TODO: move into GPIO driver (once implemented) */
    UART_PORT->SEL |= (UART_RX_PIN | UART_TX_PIN);
    msp_port_t *port = &UART_PORT->base;
    port->OD |= UART_RX_PIN;
    port->OD &= ~(UART_TX_PIN);
    port->DIR |= UART_TX_PIN;
    port->DIR &= ~(UART_RX_PIN);
    /* enable receiver and transmitter */
    uart_poweron(uart);
    /* and finally release the software reset bit */
    dev->CTL &= ~(SWRST);
    return UART_OK;
}

void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    (void)uart;
    msp_usart_t *dev = UART_BASE;

    for (size_t i = 0; i < len; i++) {
        while (!(dev->TCTL & TXEPT)) {}
        dev->TXBUF = data[i];
    }
}

void uart_poweron(uart_t uart)
{
    (void)uart;
    UART_SFR->ME |= UART_ME_BITS;
}

void uart_poweroff(uart_t uart)
{
    (void)uart;
    UART_SFR->ME &= ~(UART_ME_BITS);
}

ISR(UART_RX_ISR, isr_uart_0_rx)
{
    __enter_isr();

    /* read character (resets interrupt flag) */
    char c = UART_BASE->RXBUF;

    /* only call callback if there was no receive error */
    if(! (UART_BASE->RCTL & RXERR)) {
        ctx_rx_cb(ctx_isr_arg, c);
    }

    __exit_isr();
}
