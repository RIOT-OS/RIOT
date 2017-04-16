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
 *
 * Support static BAUD rate calculation using UART_STDIO_BAUDRATE.
 * Set UART_STDIO_BAUDRATE to the desired baud rate and pass it as a -D argument
 * at compliation time (e.g. in the boards Makefile.include file).
 * UART_BAUD_TOL can be set to guarantee a BAUD rate tolerance at compile time or
 * to switch to double speed transmission (U2X) to achieve a lower tolerance.
 * At runtime, this tolerance is not guaranteed to be met.
 * However, an error message will be displayed at compile time.
 *
 * @}
 */

#include "cpu.h"
#include "sched.h"
#include "thread.h"

#include "periph/uart.h"


/**
 * @brief   Maximum percentage error in calculated baud before switching to
 *          double speed transmission (U2X)
 *
 * Takes whole numbers from 0 to 100, inclusive, with a default of 2.
 */
#if defined(UART_BAUD_TOL)
/* BAUD_TOL is defined here as it is used by the setbaud.h utility */
#define BAUD_TOL UART_BAUD_TOL
#else
#define BAUD_TOL 2
#endif

#if defined(UART_STDIO_BAUDRATE)
/* BAUD and F_CPU are required by setbaud.h to calculated BRR */
#define BAUD UART_STDIO_BAUDRATE
#define F_CPU CLOCK_CORECLOCK
#include <util/setbaud.h>
#endif

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

static void _update_brr(uart_t uart, uint16_t brr, bool double_speed)
{
    dev[uart]->BRR = brr;
    if (double_speed) {
        dev[uart]->CSRA |= (1 << U2X0);
    }
}

static void _set_brr(uart_t uart, uint32_t baudrate)
{
    uint16_t brr;
#if defined(UART_STDIO_BAUDRATE)
    // UBRR_VALUE and USE_2X are statically computed from <util/setbaud.h>
    if (baudrate == UART_STDIO_BAUDRATE) {
        _update_brr(uart, UBRR_VALUE, USE_2X);
        return;
    }
#endif
#if defined(UART_DOUBLE_SPEED)
    brr = (CLOCK_CORECLOCK + 4UL * baudrate) / (8UL * baudrate) - 1UL;
    _update_brr(uart, brr, true);
#else
    brr = (CLOCK_CORECLOCK + 8UL * baudrate) / (16UL * baudrate) - 1UL;
    _update_brr(uart, brr, false);
#endif
}

int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg)
{
    /* make sure the given device is valid */
    if (uart >= UART_NUMOF) {
        return UART_NODEV;
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
    _set_brr(uart, baudrate);

    /* enable RX and TX and the RX interrupt */
    if (rx_cb) {
        dev[uart]->CSRB = ((1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0));
    }
    else {
        dev[uart]->CSRB = (1 << TXEN0);
    }


    return UART_OK;
}

void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    for (size_t i = 0; i < len; i++) {
        while (!(dev[uart]->CSRA & (1 << UDRE0))) {};
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
