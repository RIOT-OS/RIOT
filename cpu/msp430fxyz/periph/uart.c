/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_msp430fxyz
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
static uart_tx_cb_t ctx_tx_cb;
static void *ctx_isr_arg;
/** @} */

/* per default, we use the legacy MSP430 USART module for UART functionality */
#ifndef UART_USE_USIC

int uart_init(uart_t uart, uint32_t baudrate,
              uart_rx_cb_t rx_cb, uart_tx_cb_t tx_cb, void *arg)
{
    if (uart_init_blocking(uart, baudrate) < 0) {
        return -1;
    }

    /* save interrupt context */
    ctx_rx_cb = rx_cb;
    ctx_tx_cb = tx_cb;
    ctx_isr_arg = arg;
    /* reset interrupt flags and enable RX interrupt */
    UART_IE &= ~(UART_IE_TX_BIT);
    UART_IF &= ~(UART_IE_RX_BIT);
    UART_IF |=  (UART_IE_TX_BIT);
    UART_IE |=  (UART_IE_RX_BIT);
    return 0;
}

int uart_init_blocking(uart_t uart, uint32_t baudrate)
{
    if (uart != 0) {
        return -1;
    }

    /* get the default UART for now -> TODO: enable for multiple devices */
    msp_usart_t *dev = UART_DEV;

    /* power off and reset device */
    uart_poweroff(uart);
    dev->CTL = USART_CTL_SWRST;
    /* configure to 8N1 and using the SMCLK*/
    dev->CTL |= USART_CTL_CHAR;
    dev->TCTL = (USART_TCTL_TXEPT | USART_TCTL_SSEL_SMCLK);
    dev->RCTL = 0x00;
    /* baudrate configuration */
    uint16_t br = (uint16_t)(CLOCK_CMCLK / baudrate);
    dev->BR0 = (uint8_t)br;
    dev->BR1 = (uint8_t)(br >> 8);
    /* TODO: calculate value for modulation register */
    dev->MCTL = 0;
    /* configure pins -> TODO: move into GPIO driver (once implemented) */
    UART_PORT->SEL |= (UART_RX_PIN | UART_TX_PIN);
    UART_PORT->OD |= UART_RX_PIN;
    UART_PORT->OD &= ~(UART_TX_PIN);
    UART_PORT->DIR |= UART_TX_PIN;
    UART_PORT->DIR &= ~(UART_RX_PIN);
    /* enable receiver and transmitter */
    uart_poweron(uart);
    /* and finally release the software reset bit */
    dev->CTL &= ~(USART_CTL_SWRST);
    return 0;
}

void uart_tx_begin(uart_t uart)
{
    (void)uart;

    UART_IE |= UART_IE_TX_BIT;
}

int uart_write(uart_t uart, char data)
{
    (void)uart;
    msp_usart_t *dev = UART_DEV;

    dev->TXBUF = (uint8_t)data;
    return 1;
}

int uart_write_blocking(uart_t uart, char data)
{
    (void)uart;
    msp_usart_t *dev = UART_DEV;

    while (!(dev->TCTL & USART_TCTL_TXEPT));
    dev->TXBUF = (uint8_t)data;
    return 1;
}

int uart_read_blocking(uart_t uart, char *data)
{
    (void)uart;
    msp_usart_t *dev = UART_DEV;

    while (!(UART_IF & UART_IE_RX_BIT));
    *data = (char)dev->RXBUF;
    return 1;
}

void uart_poweron(uart_t uart)
{
    UART_ME |= UART_ME_BITS;
}

void uart_poweroff(uart_t uart)
{
    UART_ME &= ~(UART_ME_BITS);
}

ISR(UART_RX_ISR, isr_uart_0_rx)
{
    __enter_isr();

    /* read character (resets interrupt flag) */
    char c = UART_DEV->RXBUF;

    /* only call callback if there was no receive error */
    if(! (UART_DEV->RCTL & RXERR)) {
        ctx_rx_cb(ctx_isr_arg, c);
    }

    __exit_isr();
}

ISR(UART_TX_ISR, isr_uart_0_tx)
{
    __enter_isr();

    if (UART_IF & UART_IE_TX_BIT) {
        if (ctx_tx_cb(ctx_isr_arg) == 0) {
            UART_IE &= ~(UART_IE_TX_BIT);
        }
        else {
            UART_IF &= ~(UART_IE_TX_BIT);
        }
    }

    __exit_isr();
}

/* we use alternative UART code in case the board used the USIC module for UART
 * in case of the (older) USART module */
#else

int uart_init(uart_t uart, uint32_t baudrate,
              uart_rx_cb_t rx_cb, uart_tx_cb_t tx_cb, void *arg)
{
    if (uart_init_blocking(uart, baudrate) < 0) {
        return -1;
    }

    /* save interrupt context */
    ctx_rx_cb = rx_cb;
    ctx_tx_cb = tx_cb;
    ctx_isr_arg = arg;
    /* reset interrupt flags and enable RX interrupt */
    UART_IF &= ~(UART_IE_RX_BIT);
    UART_IF |=  (UART_IE_TX_BIT);
    UART_IE |=  (UART_IE_RX_BIT);
    UART_IE &= ~(UART_IE_TX_BIT);
    return 0;
}

int uart_init_blocking(uart_t uart, uint32_t baudrate)
{
    if (uart != 0) {
        return -1;
    }

    /* get the default UART for now -> TODO: enable for multiple devices */
    msp_usci_t *dev = UART_DEV;

    /* put device in reset mode while configuration is going on */
    dev->ACTL1 = USCI_ACTL1_SWRST;
    /* configure to UART, using SMCLK in 8N1 mode */
    dev->ACTL1 |= USCI_ACTL1_SSEL_SMCLK;
    dev->ACTL0 = 0;
    dev->ASTAT = 0;
    /* configure baudrate */
    uint32_t base = ((CLOCK_CMCLK << 7)  / baudrate);
    uint16_t br = (uint16_t)(base >> 7);
    uint8_t brs = (((base & 0x3f) * 8) >> 7);
    dev->ABR0 = (uint8_t)br;
    dev->ABR1 = (uint8_t)(br >> 8);
    dev->AMCTL = (brs << USCI_AMCTL_BRS_SHIFT);
    /* pin configuration -> TODO: move to GPIO driver once implemented */
    UART_RX_PORT->SEL |= UART_RX_PIN;
    UART_TX_PORT->SEL |= UART_TX_PIN;
    UART_RX_PORT->DIR &= ~(UART_RX_PIN);
    UART_TX_PORT->DIR &= ~(UART_TX_PIN);
    /* releasing the software reset bit starts the UART */
    dev->ACTL1 &= ~(USCI_ACTL1_SWRST);
    return 0;
}

void uart_tx_begin(uart_t uart)
{
    UART_IE |= (UART_IE_TX_BIT);
}

int uart_write(uart_t uart, char data)
{
    (void)uart;

    UART_DEV->ATXBUF = (uint8_t)data;
    return 1;
}

int uart_write_blocking(uart_t uart, char data)
{
    (void)uart;

    while (!(UART_IF & UART_IE_TX_BIT));
    UART_DEV->ATXBUF = (uint8_t)data;
    return 1;
}

int uart_read_blocking(uart_t uart, char *data)
{
    (void)uart;

    while (!(UART_IF & UART_IE_RX_BIT));
    *data = (char)UART_DEV->ARXBUF;
    return 1;
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

    uint8_t stat = UART_DEV->ASTAT;
    char data = (char)UART_DEV->ARXBUF;

    if (stat & (USCI_ASTAT_FE | USCI_ASTAT_OE | USCI_ASTAT_PE | USCI_ASTAT_BRK)) {
        /* some error which we do not handle, just do a pseudo read to reset the
         * status register */
        (void)data;
    }
    else {
        ctx_rx_cb(ctx_isr_arg, data);
    }

    __exit_isr();
}

ISR(UART_TX_ISR, isr_uart0_tx)
{
    __enter_isr();

    if (ctx_tx_cb(ctx_isr_arg) == 0) {
        UART_IE &= ~(UART_IE_TX_BIT);
    }
    else {
        UART_IF &= ~(UART_IE_TX_BIT);
    }

    __exit_isr();
}

#endif
