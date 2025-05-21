/*
 * Copyright (C) 2015 Freie Universität Berlin
 *               2023 Otto-von-Guericke-Universität Magdeburg
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
 * @author      Marian Buschsieweke <marian.buschsieweke@posteo.net>
 *
 * @}
 */

#include "cpu.h"
#include "periph_cpu.h"
#include "periph_conf.h"
#include "periph/uart.h"
#include "periph/gpio.h"
#include "compiler_hints.h"

static uart_isr_ctx_t ctx[UART_NUMOF];
static msp430_usart_conf_t confs[UART_NUMOF];

static void init(uart_t uart)
{
    const msp430_usart_uart_params_t *params = uart_config[uart].uart;
    msp430_usart_t *dev = params->usart_params.dev;
    uint8_t enable_mask = params->rxtx_enable_mask;

    /* most of the time UART is used to both send and receive */
    if (unlikely(!ctx[uart].rx_cb)) {
        enable_mask = params->tx_enable_mask;
    }

    /* acquire USART and put in in UART mode */
    msp430_usart_acquire(&params->usart_params, &confs[uart], enable_mask);

    /* configure pins */
    gpio_set(params->txd);
    gpio_init(params->txd, GPIO_OUT);
    gpio_periph_mode(params->txd, true);

    gpio_init(params->rxd, GPIO_IN);
    gpio_periph_mode(params->rxd, true);

    /* now that everything is configured, release the software reset bit */
    dev->CTL &= ~(SWRST);

    /* finally, enable the RX IRQ (won't work prior to releasing the software
     * reset bit, as this is cleared under reset) */
    if (likely(ctx[uart].rx_cb)) {
        msp430_usart_enable_rx_irq(&params->usart_params);
    }
}

int uart_init(uart_t uart, uint32_t symbolrate, uart_rx_cb_t rx_cb, void *arg)
{
    assume((unsigned)uart < UART_NUMOF);

    /* save interrupt context */
    ctx[uart].rx_cb = rx_cb;
    ctx[uart].arg = arg;

    /* prepare and save UART config */
    confs[uart].ctl = CHAR;
    confs[uart].prescaler = msp430_usart_prescale(symbolrate, USART_MIN_BR_UART);

    if (rx_cb) {
        init(uart);
    }

    return UART_OK;
}

void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    assume((unsigned)uart < UART_NUMOF);
    const msp430_usart_uart_params_t *params = uart_config[uart].uart;
    msp430_usart_t *dev = params->usart_params.dev;

    /* If UART is in TX-only, the USART is released when done sending.
     * This is not only helpful for power saving, but also to share the USART.
     */
    if (!unlikely(ctx[uart].rx_cb)) {
        init(uart);
    }

    for (size_t i = 0; i < len; i++) {
        while (!(dev->TCTL & TXEPT)) {}
        dev->TXBUF = data[i];
    }

    if (!unlikely(ctx[uart].rx_cb)) {
        msp430_usart_release(&params->usart_params);
    }
}

void uart_poweron(uart_t uart)
{
    assume((unsigned)uart < UART_NUMOF);
    /* In TX-only mode, the USART is only powered on when transmitting */
    if (likely(ctx[uart].rx_cb)) {
        init(uart);
    }
}

void uart_poweroff(uart_t uart)
{
    assume((unsigned)uart < UART_NUMOF);
    const msp430_usart_uart_params_t *params = uart_config[uart].uart;
    /* In TX-only mode, the USART is only powered on when transmitting */
    if (likely(ctx[uart].rx_cb)) {
        msp430_usart_release(&params->usart_params);
    }
}

static void uart_rx_isr(uart_t uart)
{
    assume((unsigned)uart < UART_NUMOF);
    const msp430_usart_uart_params_t *params = uart_config[uart].uart;
    msp430_usart_t *dev = params->usart_params.dev;

    /* read character (resets interrupt flag) */
    char c = dev->RXBUF;

    /* only call callback if there was no receive error */
    if (!(dev->RCTL & RXERR)) {
        ctx[uart].rx_cb(ctx[uart].arg, c);
    }
}

#ifdef UART0_RX_ISR
ISR(UART0_RX_ISR, isr_uart_0_rx)
{
    __enter_isr();
    uart_rx_isr(UART_DEV(0));
    __exit_isr();
}
#endif

#ifdef UART1_RX_ISR
ISR(UART1_RX_ISR, isr_uart_0_rx)
{
    __enter_isr();
    uart_rx_isr(UART_DEV(1));
    __exit_isr();
}
#endif
