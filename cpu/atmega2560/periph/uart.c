/*
 * Copyright (C) 2014 Freie Universität Berlin, Hinnerk van Bruinehsen
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
 * @author      Hinnerk van Bruinehsen <h.v.bruinehsen@fu-berlin.de>
 *
 * @}
 */

#include "cpu.h"
#include "sched.h"
#include "thread.h"

#include "periph/uart.h"

/**
 * @brief   Configured device map
 * @{
 */
#if UART_NUMOF
static mega_uart_t *dev[] = {
#ifdef UART_0
    UART_0,
#endif
#ifdef UART_1
    UART_1,
#endif
#ifdef UART_2
    UART_2,
#endif
#ifdef UART_3
    UART_3
#endif
};
#else
/* fallback if no UART is defined */
static const mega_uart_t *dev[] = { NULL };
#endif

/**
 * @brief   Allocate memory to store the callback functions.
 */
static uart_isr_ctx_t isr_ctx[UART_NUMOF];

int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg)
{
    /* make sure the given device is valid */
    if (uart >= UART_NUMOF) {
        return -1;
    }

    /* register interrupt context */
    isr_ctx[uart].rx_cb = rx_cb;
    isr_ctx[uart].arg   = arg;

    /* disable and reset UART */
    dev[uart]->CSRB = 0;
    dev[uart]->CSRA = 0;

    /* configure UART to 8N1 mode */
    dev[uart]->CSRC = (1 << UCSZ00) | (1 << UCSZ01);
    /* set clock divider */
    dev[uart]->BRR = CLOCK_CORECLOCK / (16 * baudrate);
    /* enable RX and TX and the RX interrupt */
    dev[uart]->CSRB = ((1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0));

    return 0;
}

void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    for (size_t i = 0; i < len; i++) {
        while (!(dev[uart]->CSRA & (1 << UDRE0)));
        dev[uart]->DR = data[i];
    }
}

static inline void isr_handler(int num)
{
    isr_ctx[num].rx_cb(isr_ctx[num].arg, dev[num]->DR);

    if (sched_context_switch_request) {
        thread_yield();
    }
}

#ifdef UART_0_ISR
ISR(UART_0_ISR, ISR_BLOCK)
{
    __enter_isr();
    isr_handler(0);
    __exit_isr();
}
#endif /* UART_0_ISR */

#ifdef UART_1_ISR
ISR(UART_1_ISR, ISR_BLOCK)
{
    __enter_isr();
    isr_handler(1);
    __exit_isr();
}
#endif /* UART_1_ISR */

#ifdef UART_2_ISR
ISR(UART_2_ISR, ISR_BLOCK)
{
    __enter_isr();
    isr_handler(2);
    __exit_isr();
}
#endif /* UART_2_ISR */

#ifdef UART_3_ISR
ISR(UART_3_ISR, ISR_BLOCK)
{
    __enter_isr();
    isr_handler(3);
    __exit_isr();
}
#endif /* UART_3_ISR */
