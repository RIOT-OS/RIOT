/*
 * Copyright (C) 2014 Freie Universität Berlin, Hinnerk van Bruinehsen
 *               2017 Thomas Perrot <thomas.perrot@tupi.fr>
 *               2023 Hugues Larrive
 *               2023 Gerson Fernando Budke
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
 * @author      Thomas Perrot <thomas.perrot@tupi.fr>
 * @author      Hugues Larrive <hugues.larrive@pm.me>
 * @author      Gerson Fernando Budke <nandojve@gmail.com>
 *
 *
 * Support static BAUD rate calculation using STDIO_UART_BAUDRATE.
 * Set STDIO_UART_BAUDRATE to the desired baud rate and pass it as a -D argument
 * at compilation time (e.g. in the boards Makefile.include file).
 * UART_BAUD_TOL can be set to guarantee a BAUD rate tolerance at compile time or
 * to switch to double speed transmission (U2X) to achieve a lower tolerance.
 * At runtime, this tolerance is not guaranteed to be met.
 * However, an error message will be displayed at compile time.
 *
 * @}
 */

#include "cpu.h"
#include "irq.h"
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

#if defined(STDIO_UART_BAUDRATE)
/* BAUD and F_CPU are required by setbaud.h to calculated BRR */
#define BAUD STDIO_UART_BAUDRATE
#define F_CPU CLOCK_CORECLOCK
#include <util/setbaud.h>
#endif

/**
 * @brief   Configured device map
 */
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

/**
 * @brief   Allocate memory to store the callback functions.
 */
static uart_isr_ctx_t isr_ctx[UART_NUMOF];

static void _update_brr(uart_t uart, uint16_t brr, bool double_speed)
{
#if defined(UCSR0A) || defined(UCSR1A)
    dev[uart]->BRR = brr;
#elif defined(UCSRA) /* atmega8 */
    /* on atmega8 BRRH is shared with CSRC */
    dev[uart]->CSRC = (brr >> 8);
    dev[uart]->BRRL = (uint8_t)(brr & 0x00ff);
#endif
    if (double_speed) {
#if defined(U2X) /* atmega8 */
        dev[uart]->CSRA |= (1 << U2X);
#elif defined(U2X0)
        dev[uart]->CSRA |= (1 << U2X0);
#elif defined(U2X1) /* atmega32u4 */
        dev[uart]->CSRA |= (1 << U2X1);
#endif
    }
}

static void _set_brr(uart_t uart, uint32_t baudrate)
{
    uint16_t brr;

#if defined(STDIO_UART_BAUDRATE)
    /* UBRR_VALUE and USE_2X are statically computed from <util/setbaud.h> */
    if (baudrate == STDIO_UART_BAUDRATE) {
        _update_brr(uart, UBRR_VALUE, USE_2X);
        return;
    }
#endif
/* brr calculation is different from the datasheet to provide better rounding */
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

    uint16_t count = UINT16_MAX;
    while (avr8_is_uart_tx_pending() && count--) {}

    /* register interrupt context */
    isr_ctx[uart].rx_cb = rx_cb;
    isr_ctx[uart].arg = arg;

    /* disable and reset UART */
#ifdef UCSR1D /* 32u4 */
    dev[uart]->CSRD = 0;
#endif
    dev[uart]->CSRB = 0;
    dev[uart]->CSRA = 0;

    /* configure UART to 8N1 mode */
#if defined(UCSZ0) && defined(UCSZ1) /* atmega8 */
    dev[uart]->CSRC = (1 << UCSZ0) | (1 << UCSZ1);
#elif defined(UCSZ00) && defined(UCSZ01)
    dev[uart]->CSRC = (1 << UCSZ00) | (1 << UCSZ01);
#elif defined(UCSZ10) && defined(UCSZ11) /* 32u4 */
    dev[uart]->CSRC = (1 << UCSZ10) | (1 << UCSZ11);
#endif
    /* set clock divider */
    _set_brr(uart, baudrate);

    /* enable RX and TX and their respective interrupt */
    if (rx_cb) {
#if defined(RXCIE) /* atmega8 */
        dev[uart]->CSRB = ((1 << RXCIE) | (1 << TXCIE) | (1 << RXEN) | (1 << TXEN));
#elif defined(RXCIE0)
        dev[uart]->CSRB = ((1 << RXCIE0) | (1 << TXCIE0) | (1 << RXEN0) | (1 << TXEN0));
#elif defined(RXCIE1) /* 32u4 */
        dev[uart]->CSRB = ((1 << RXCIE1) | (1 << TXCIE1) | (1 << RXEN1) | (1 << TXEN1));
#endif
    }
    else {
#if defined(TXEN) /* atmega8 */
        dev[uart]->CSRB = ((1 << TXEN) | (1 << TXCIE));
#elif defined(TXEN0)
        dev[uart]->CSRB = ((1 << TXEN0) | (1 << TXCIE0));
#elif defined(TXEN1) /* 32u4 */
        dev[uart]->CSRB = ((1 << TXEN1) | (1 << TXCIE1));
#endif
    }

    return UART_OK;
}

void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    for (size_t i = 0; i < len; i++) {
#if defined(UDRE) /* atmega8 */
        while (!(dev[uart]->CSRA & (1 << UDRE))) {};
#elif defined(UDRE0)
        while (!(dev[uart]->CSRA & (1 << UDRE0))) {}
#elif defined(UDRE1) /* 32u4 */
        while (!(dev[uart]->CSRA & (1 << UDRE1))) {};
#endif
        /* start of TX won't finish until no data in UDRn and transmit shift
           register is empty */
        unsigned long state = irq_disable();
        avr8_uart_tx_set_pending(uart);
        irq_restore(state);

        dev[uart]->DR = data[i];
    }
}

void uart_poweron(uart_t uart)
{
    (void)uart;
    /* not implemented (yet) */
}

void uart_poweroff(uart_t uart)
{
    (void)uart;
    /* not implemented (yet) */
}

static inline void _rx_isr_handler(int num)
{
    isr_ctx[num].rx_cb(isr_ctx[num].arg, dev[num]->DR);
}

static inline void _tx_isr_handler(int num)
{
    /* entire frame in the Transmit Shift Register has been shifted out and
       there are no new data currently present in the transmit buffer */
    avr8_uart_tx_clear_pending(num);
}

#ifdef UART_0_ISR
AVR8_ISR(UART_0_ISR, _rx_isr_handler, 0);
#endif /* UART_0_ISR */

#ifdef UART_1_ISR
AVR8_ISR(UART_1_ISR, _rx_isr_handler, 1);
#endif /* UART_1_ISR */

#ifdef UART_2_ISR
AVR8_ISR(UART_2_ISR, _rx_isr_handler, 2);
#endif /* UART_2_ISR */

#ifdef UART_3_ISR
AVR8_ISR(UART_3_ISR, _rx_isr_handler, 3);
#endif /* UART_3_ISR */

#ifdef UART_0_ISR_TX
AVR8_ISR(UART_0_ISR_TX, _tx_isr_handler, 0);
#endif /* UART_0_ISR_TX */

#ifdef UART_1_ISR_TX
AVR8_ISR(UART_1_ISR_TX, _tx_isr_handler, 1);
#endif /* UART_1_ISR_TX */

#ifdef UART_2_ISR_TX
AVR8_ISR(UART_2_ISR_TX, _tx_isr_handler, 2);
#endif /* UART_2_ISR_TX */

#ifdef UART_3_ISR_TX
AVR8_ISR(UART_3_ISR_TX, _tx_isr_handler, 3);
#endif /* UART_3_ISR_TX */
