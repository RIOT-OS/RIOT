/*
 * Copyright (C) 2014 Freie Universität Berlin, Hinnerk van Bruinehsen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_atmega_common
 * @ingroup     drivers_periph_uart
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

#ifdef MODULE_PM_LAYERED
#include "pm_layered.h"
#endif

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
 * @brief   Stores context information for each uart
 */
typedef struct {
    mega_uart_t *dev;          /**< uart device */
    int8_t num;                /** < hardware timer number */
} ctx_t;

#if UART_NUMOF
static ctx_t ctx[] = {
#ifdef UART_0
    {UART_0, -1},
#endif
#ifdef UART_1
    {UART_1, -1},
#endif
#ifdef UART_2
    {UART_2, -1},
#endif
#ifdef UART_3
    {UART_3, -1}
#endif
};
#else
/* fallback if no UART is defined */
static const ctx_t ctx[] = { NULL };
#endif

/**
 * @brief   Allocate memory to store the callback functions.
 */
static uart_isr_ctx_t isr_ctx[UART_NUMOF];

static void _update_brr(uart_t uart, uint16_t brr, bool double_speed)
{
    ctx[uart].dev->BRR = brr;
    if (double_speed) {
        ctx[uart].dev->CSRA |= (1 << U2X0);
    }
}

static void _flush(uart_t uart) {
    uint8_t dummy;
    mega_uart_t* dev = ctx[uart].dev;
    /* Clears the RX Buffer, but nothing else! */
    /* TODO: Do not use RXC0 for UART1, ... - is this even a problem? */
    while ( dev->CSRA & (1<< RXC0) ) {
        dummy = ctx[uart].dev->DR;
    }
    (void)dummy;
    /* Clears the transmit buffer */
    //while (!(ctx[uart].dev->CSRA & (1 << UDRE0))) { };
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

    /* TODO: What about just UART (without any number?) */
    #ifdef MEGA_UART0
    if(ctx[uart].dev == MEGA_UART0) {
        ctx[uart].num = 0;
    } else
    #endif
    #ifdef MEGA_UART1
    if(ctx[uart].dev == MEGA_UART1) {
        ctx[uart].num = 1;
    } else
    #endif
    #ifdef MEGA_UART2
    if(ctx[uart].dev == MEGA_UART3) {
        ctx[uart].num = 2;
    } else
    #endif
    #ifdef MEGA_UART3
    if(ctx[uart].dev == MEGA_UART3) {
        ctx[uart].num = 3;
    } else
    #endif

    if (ctx[uart].num == -1 ) {
        return -1;
    }

    /* disable and reset UART */
    ctx[uart].dev->CSRB = 0;
    ctx[uart].dev->CSRA = 0;

    /* configure UART to 8N1 mode */
    ctx[uart].dev->CSRC = (1 << UCSZ00) | (1 << UCSZ01);
    /* Power on uart so we can use it */
    uart_poweron(uart);
    /* set clock divider */
    _set_brr(uart, baudrate);

    return UART_OK;
}

void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    for (size_t i = 0; i < len; i++) {
        while (!(ctx[uart].dev->CSRA & (1 << UDRE0))) {};
        ctx[uart].dev->DR = data[i];
    }
}

static inline void isr_handler(int num)
{
    isr_ctx[num].rx_cb(isr_ctx[num].arg, ctx[num].dev->DR);

    if (sched_context_switch_request) {
        thread_yield();
    }
}

void uart_poweron(uart_t uart) {
    switch(ctx[uart].num) {
#ifdef MEGA_UART0
    case 0 :
    #ifdef MODULE_PM_LAYERED
        pm_block(PM_INVALID_UART0);
    #endif
        power_usart0_enable();
    break;
#endif
#ifdef MEGA_UART1
    case 1 :
    #ifdef MODULE_PM_LAYERED
        pm_block(PM_INVALID_UART1);
    #endif
        power_usart1_enable();
    break;
#endif
    }

    /* enable RX and TX and the RX interrupt */
    if (isr_ctx[uart].rx_cb) {
        ctx[uart].dev->CSRB = ((1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0));
    }
    else {
        ctx[uart].dev->CSRB = (1 << TXEN0);
    }
}

void uart_poweroff(uart_t uart) {
    _flush(uart);

    if (isr_ctx[uart].rx_cb) {
        ctx[uart].dev->CSRB &= ~((1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0));
    }
    else {
        ctx[uart].dev->CSRB &= ~(1 << TXEN0);
    }

    switch(ctx[uart].num) {
#ifdef MEGA_UART0
    case 0 :
        power_usart0_disable();
    #ifdef MODULE_PM_LAYERED
        pm_unblock(PM_INVALID_UART0);
    #endif
    break;
#endif
#ifdef MEGA_UART1
    case 1 :
        power_usart1_disable();
    #ifdef MODULE_PM_LAYERED
        pm_unblock(PM_INVALID_UART1);
    #endif
    break;
#endif
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
