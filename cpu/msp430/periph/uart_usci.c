/*
 * SPDX-FileCopyrightText: 2015 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2024 Marian Buschsieweke
 * SPDX-License-Identifier: LGPL-2.1-only
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
 * @author      Marian Buschsieweke <marian.buschsieweke@posteo.net>
 *
 * @}
 */

#include "compiler_hints.h"
#include "cpu.h"
#include "irq.h"
#include "periph/gpio.h"
#include "periph/uart.h"
#include "periph_conf.h"
#include "periph_cpu.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static uart_isr_ctx_t _isr_ctx[UART_NUMOF];
static msp430_usci_conf_t _confs[UART_NUMOF];

static void uart_rx_isr(uintptr_t uart);

void uart_init_pins(uart_t uart)
{
    assume((unsigned)uart < UART_NUMOF);

    const msp430_usci_uart_params_t *params = uart_config[uart].uart;

    gpio_set(params->txd);
    gpio_init(params->txd, GPIO_OUT);
    gpio_periph_mode(params->txd, true);

    gpio_init(params->rxd, GPIO_IN);
    gpio_periph_mode(params->rxd, true);
}

void uart_deinit_pins(uart_t uart)
{
    assume((unsigned)uart < UART_NUMOF);

    const msp430_usci_uart_params_t *params = uart_config[uart].uart;

    gpio_init(params->txd, GPIO_IN);
    gpio_periph_mode(params->txd, false);

    gpio_init(params->rxd, GPIO_IN);
    gpio_periph_mode(params->rxd, false);
}

static void _init(uart_t uart)
{
    const msp430_usci_uart_params_t *params = uart_config[uart].uart;
    const msp430_usci_conf_t *conf = &_confs[uart];
    uint8_t enable_rx_irq = 0;
    /* enable RX IRQ, if callback function is set */
    if (_isr_ctx[uart].rx_cb) {
        enable_rx_irq = params->usci_params.rx_irq_mask;
    }
    /* acquire and configure USCI */
    msp430_usci_acquire(&params->usci_params, conf);
    /* reset error stats */
    params->usci_params.dev->STAT = 0;
    /* release USCI from reset and enable RX IRQ */
    params->usci_params.dev->CTL1 &= ~(UCSWRST);
    /* interrupt enable register is shared between two USCI peripherals, hence
     * the other may be concurrently be configured */
    unsigned irq_state = irq_disable();
    *params->usci_params.interrupt_enable |= enable_rx_irq;
    irq_restore(irq_state);
}

int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg)
{
    assume((unsigned)uart < UART_NUMOF);

    /* prepare configuration */
    _confs[uart] = (msp430_usci_conf_t){
        .prescaler = msp430_usci_prescale(baudrate),
        .ctl0 = 0,
    };

    /* save interrupt context */
    _isr_ctx[uart] = (uart_isr_ctx_t){
        .rx_cb = rx_cb,
        .arg = arg,
    };

    /* prepare pins */
    uart_init_pins(uart);

    /* only enable USCI if RX is used, otherwise we enable it on-demand to
     * allow sharing the USCI and conserve power */

    if (rx_cb) {
        _init(uart);
    }

    return 0;
}

void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    assume((unsigned)uart < UART_NUMOF);

    const msp430_usci_params_t *usci = &uart_config[uart].uart->usci_params;
    bool tx_only = !_isr_ctx[uart].rx_cb;

    /* in TX-only mode, we enable the USCI on-demand */
    if (tx_only) {
        _init(uart);
    }

    while (len--) {
        while (!(*usci->interrupt_flag & usci->tx_irq_mask)) { }
        usci->dev->TXBUF = *data++;
    }

    while (usci->dev->STAT & UCBUSY) {
        /* busy wait for completion, e.g. to avoid losing chars/bits
         * before releasing the USCI in TX only mode. */
    }

    if (tx_only) {
        msp430_usci_release(usci);
    }
}

void uart_poweron(uart_t uart)
{
    assume((unsigned)uart < UART_NUMOF);

    if (!_isr_ctx[uart].rx_cb) {
        /* in TX only mode, the USCI will only be turned on on-demand anyway */
        return;
    }

    _init(uart);
}

void uart_poweroff(uart_t uart)
{
    assume((unsigned)uart < UART_NUMOF);

    if (!_isr_ctx[uart].rx_cb) {
        /* in TX only mode, the USCI will only be turned on on-demand anyway */
        return;
    }

    const msp430_usci_params_t *usci = &uart_config[uart].uart->usci_params;

    msp430_usci_release(usci);
}

static void uart_rx_isr(uintptr_t uart)
{
    assume((unsigned)uart < UART_NUMOF);
    const msp430_usci_params_t *usci = &uart_config[uart].uart->usci_params;
    uint8_t stat = usci->dev->STAT;
    uint8_t data = (uint8_t)usci->dev->RXBUF;

    if (stat & (UCFE | UCOE | UCPE | UCBRK)) {
        /* TODO: Add proper error handling */
        usci->dev->STAT = 0;
        DEBUG("[uart@%u] Error: %04x\n", (unsigned)uart, (unsigned)stat);
    }
    else {
        _isr_ctx[uart].rx_cb(_isr_ctx[uart].arg, data);
    }

}

/* only USCI A0 and USCI A1 can be used for UARTs, so at most two ISRS needed */
#ifdef UART0_RX_ISR
ISR(UART0_RX_ISR, isr_uart0)
{
    __enter_isr();
    uart_rx_isr(0);
    __exit_isr();
}
#endif

#ifdef UART1_RX_ISR
ISR(UART1_RX_ISR, isr_uart1)
{
    __enter_isr();
    uart_rx_isr(1);
    __exit_isr();
}
#endif
