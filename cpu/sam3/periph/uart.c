/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_periph
 * @{
 *
 * @file
 * @brief       Low-level UART driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "board.h"
#include "cpu.h"
#include "sched.h"
#include "thread.h"
#include "periph/uart.h"
#include "periph_conf.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/**
 * @brief Allocate memory to store the callback functions.
 */
static uart_isr_ctx_t uart_config[UART_NUMOF];

static int init_base(uart_t uart, uint32_t baudrate);

int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg)
{
    /* initialize basic functionality */
    int res = init_base(uart, baudrate);
    if (res != 0) {
        return res;
    }

    /* register callbacks */
    uart_config[uart].rx_cb = rx_cb;
    uart_config[uart].arg = arg;

    /* configure interrupts and enable RX interrupt */
    switch (uart) {
#if UART_0_EN
        case UART_0:
            NVIC_EnableIRQ(UART_0_IRQ);
            UART_0_DEV->UART_IER = UART_IER_RXRDY;
            break;
#endif
#if UART_1_EN
        case UART_1:
            NVIC_EnableIRQ(UART_1_IRQ);
            UART_1_DEV->US_IER = US_IER_RXRDY;
            break;
#endif
#if UART_2_EN
        case UART_2:
            NVIC_EnableIRQ(UART_2_IRQ);
            UART_2_DEV->US_IER = US_IER_RXRDY;
            break;
#endif
#if UART_3_EN
        case UART_3:
            NVIC_EnableIRQ(UART_3_IRQ);
            UART_3_DEV->US_IER = US_IER_RXRDY;
            break;
#endif
    }
    return 0;
}

static int init_base(uart_t uart, uint32_t baudrate)
{
    switch (uart) {
#if UART_0_EN
        case UART_0:
            /* enable uart clock */
            UART_0_CLKEN();

            /* configure PINS */
            UART_0_PORT->PIO_PDR = UART_0_PINS;
            UART_0_PORT->PIO_ABSR &= ~UART_0_PINS;      /* periph function A */

            /* set clock divider */
            UART_0_DEV->UART_BRGR = (F_CPU / (16 * baudrate));
            /* set to normal mode without parity */
            UART_0_DEV->UART_MR = UART_MR_PAR_NO | UART_MR_CHMODE_NORMAL;
            /* enable receiver and transmitter and reset status bits */
            UART_0_DEV->UART_CR = UART_CR_RXEN | UART_CR_TXEN | UART_CR_RSTSTA;
            break;
#endif
#if UART_1_EN
        case UART_1:
            /* enable uart clock */
            UART_1_CLKEN();

            /* configure PINS */
            UART_1_PORT->PIO_PDR = UART_1_PINS;
            UART_1_PORT->PIO_ABSR &= ~UART_1_PINS;      /* periph function A */

            /* set clock divider */
            UART_1_DEV->US_BRGR = (F_CPU / (16 * baudrate));
            /* set to normal mode without parity */
            UART_1_DEV->US_MR = US_MR_CHRL_8_BIT | US_MR_PAR_NO;
            /* enable receiver and transmitter and reset status bits */
            UART_1_DEV->US_CR = US_CR_RXEN | US_CR_TXEN | US_CR_RSTSTA;
            break;
#endif
#if UART_2_EN
        case UART_2:
            /* enable uart clock */
            UART_2_CLKEN();

            /* configure PINS */
            UART_2_PORT->PIO_PDR = UART_2_PINS;
            UART_2_PORT->PIO_ABSR &= ~UART_2_PINS;      /* periph function A */

            /* set clock divider */
            UART_2_DEV->US_BRGR = (F_CPU / (16 * baudrate));
            /* set to normal mode without parity */
            UART_2_DEV->US_MR = US_MR_CHRL_8_BIT | US_MR_PAR_NO;
            /* enable receiver and transmitter and reset status bits */
            UART_2_DEV->US_CR = US_CR_RXEN | US_CR_TXEN | US_CR_RSTSTA;
            break;
#endif
#if UART_3_EN
        case UART_3:
            /* enable uart clock */
            UART_3_CLKEN();

            /* configure PINS */
            UART_3_PORT->PIO_PDR = UART_3_PINS;
            UART_3_PORT->PIO_ABSR |= UART_3_PINS;       /* periph function B */

            /* set clock divider */
            UART_3_DEV->US_BRGR = (F_CPU / (16 * baudrate));
            /* set to normal mode without parity */
            UART_3_DEV->US_MR = US_MR_CHRL_8_BIT | US_MR_PAR_NO;
            /* enable receiver and transmitter and reset status bits */
            UART_3_DEV->US_CR = US_CR_RXEN | US_CR_TXEN | US_CR_RSTSTA;
            break;
#endif
    }
    return 0;
}

void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    Uart *dev;

    switch (uart) {
#if UART_0_EN
        case UART_0:
            dev = (Uart *)UART_0_DEV;
            break;
#endif
#if UART_1_EN
        case UART_1:
            dev = (Uart *)UART_1_DEV;
            break;
#endif
#if UART_2_EN
        case UART_2:
            dev = (Uart *)UART_2_DEV;
            break;
#endif
#if UART_3_EN
        case UART_3:
            dev = (Uart *)UART_3_DEV;
            break;
#endif
        default:
            return;
    }

    for (size_t i = 0; i < len; i++) {
        while (!(dev->UART_SR & UART_SR_TXRDY));
        dev->UART_THR = data[i];
    }
}

void uart_poweron(uart_t uart)
{
    switch (uart) {
#if UART_0_EN
        case UART_0:
            UART_0_CLKEN();
            break;
#endif
#if UART_1_EN
        case UART_1:
            UART_1_CLKEN();
            break;
#endif
#if UART_2_EN
        case UART_2:
            UART_2_CLKEN();
            break;
#endif
#if UART_3_EN
        case UART_3:
            UART_3_CLKEN();
            break;
#endif
    }
}

void uart_poweroff(uart_t uart)
{
    switch (uart) {
#if UART_0_EN
        case UART_0:
            UART_0_CLKDIS();
            break;
#endif
#if UART_1_EN
        case UART_1:
            UART_1_CLKDIS();
            break;
#endif
#if UART_2_EN
        case UART_2:
            UART_2_CLKDIS();
            break;
#endif
#if UART_3_EN
        case UART_3:
            UART_3_CLKDIS();
            break;
#endif
    }
}

#if UART_0_EN
void UART_0_ISR(void)
{
    if (UART_0_DEV->UART_SR & UART_SR_RXRDY) {
        char data = (char)UART_0_DEV->UART_RHR;
        uart_config[UART_0].rx_cb(uart_config[UART_0].arg, data);
    }
    if (sched_context_switch_request) {
        thread_yield();
    }
}
#endif

#if UART_1_EN
void UART_1_ISR(void)
{
    if (UART_1_DEV->US_CSR & US_CSR_RXRDY) {
        char data = (char)UART_1_DEV->US_RHR;
        uart_config[UART_1].rx_cb(uart_config[UART_1].arg, data);
    }
    if (sched_context_switch_request) {
        thread_yield();
    }
}
#endif

#if UART_2_EN
void UART_2_ISR(void)
{
    if (UART_2_DEV->US_CSR & US_CSR_RXRDY) {
        char data = (char)UART_2_DEV->US_RHR;
        uart_config[UART_2].rx_cb(uart_config[UART_2].arg, data);
    }
    if (sched_context_switch_request) {
        thread_yield();
    }
}
#endif

#if UART_3_EN
void UART_3_ISR(void)
{
    if (UART_3_DEV->US_CSR & US_CSR_RXRDY) {
        char data = (char)UART_3_DEV->US_RHR;
        uart_config[UART_3].rx_cb(uart_config[UART_3].arg, data);
    }
    if (sched_context_switch_request) {
        thread_yield();
    }
}
#endif
