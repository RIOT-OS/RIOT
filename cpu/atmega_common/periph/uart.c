/*
 * Copyright (C) 2014 Freie Universität Berlin, Hinnerk van Bruinehsen
 *               2018 Acutam Automation, LLC
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
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
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

#include <avr/power.h>

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

#if defined(UART_STDIO_BAUDRATE)
/* BAUD and F_CPU are required by setbaud.h to calculated BRR */
#define BAUD UART_STDIO_BAUDRATE
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
 * @brief   Allocate memory to store the settings.
 */
static uart_isr_ctx_t isr_ctx[UART_NUMOF];
static uint16_t uart_brr[UART_NUMOF];

static inline uint8_t _is_asleep(uart_t uart)
{
#if defined(CPU_ATMEGA328P)
    (void)uart;
    return ((PRR >> PRUSART0) & 0x1);
#else

    if (uart == 0) {
        return ((PRR0 >> PRUSART0) & 0x1);
    }

#if defined(CPU_ATMEGA1284P)
    else if (uart == 1) {
        /* ATmega1284P uses PRR0 instead of PRR1 */
        return ((PRR0 >> PRUSART1) & 0x1);
    }
#else
    else if (uart == 1) {
        return ((PRR1 >> PRUSART1) & 0x1);
    }
#endif /* defined(CPU_ATMEGA1284P) */

#if (UART_NUMOF > 2)
    else if (uart == 2) {
        return ((PRR1 >> PRUSART2) & 0x1);
    }
#endif

#if (UART_NUMOF > 3)
    else if (uart == 3) {
        return ((PRR1 >> PRUSART3) & 0x1);
    }
#endif

#endif /* defined(CPU_ATMEGA328P) */

    /* extra return should be unused */
    return 0;
}

static inline void _update_brr(uart_t uart)
{
    dev[uart]->BRR = uart_brr[uart];

#if defined(UART_DOUBLE_SPEED)
    dev[uart]->CSRA |= (1 << U2X0);
#endif
}

static inline void _calc_brr(uart_t uart, uint32_t baudrate)
{
#if defined(UART_STDIO_BAUDRATE)
    /* UBRR_VALUE and USE_2X are statically computed from <util/setbaud.h> */
    if (baudrate == UART_STDIO_BAUDRATE) {
        uart_brr[uart] = UBRR_VALUE;
        return;
    }
#endif
#if defined(UART_DOUBLE_SPEED)
    uart_brr[uart] = (CLOCK_CORECLOCK + 4UL * baudrate) / (8UL * baudrate) - 1UL;
#else
    uart_brr[uart] = (CLOCK_CORECLOCK + 8UL * baudrate) / (16UL * baudrate) - 1UL;
#endif
}

static void _apply_settings(uart_t uart)
{
    /* disable and reset UART */
    dev[uart]->CSRB = 0;
    dev[uart]->CSRA = 0;

    /* configure UART to 8N1 mode */
    dev[uart]->CSRC = (1 << UCSZ00) | (1 << UCSZ01);

    /* set clock divider */
    _update_brr(uart);

    /* enable RX and TX and the RX interrupt */
    if (isr_ctx[uart].rx_cb) {
        dev[uart]->CSRB = ((1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0));
    }
    else {
        dev[uart]->CSRB = (1 << TXEN0);
    }
}

int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg)
{
    /* make sure the given device is valid */
    if (uart >= UART_NUMOF) {
        return UART_NODEV;
    }

    /* register interrupt context */
    isr_ctx[uart].rx_cb = rx_cb;
    isr_ctx[uart].arg = arg;

    /* calculate clock divider */
    _calc_brr(uart, baudrate);

    /* apply the settings to the UART */
    _apply_settings(uart);

    return UART_OK;
}

void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    uint8_t sleepy = _is_asleep(uart);

    if (sleepy) {
        uart_poweron(uart);
    }

    for (size_t i = 0; i < len; i++) {
        while (!(dev[uart]->CSRA & (1 << UDRE0))) {}
        dev[uart]->DR = data[i];
    }

    if (sleepy) {
        uart_poweroff(uart);
    }
}

void uart_poweron(uart_t uart)
{
    unsigned state = irq_disable();

    switch (uart) {
        case 0: power_usart0_enable();
#if (UART_NUMOF == 2)
        case 1: power_usart1_enable();
#endif
#if (UART_NUMOF == 3)
        case 2: power_usart2_enable();
#endif
#if (UART_NUMOF == 4)
        /* workaround missing library function */
        case 3: PRR1 &= ~(1 << PRUSART3);
#endif
        default: return;
    }

    /* bring UART to its state prior to sleeping */
    _apply_settings(uart);

    /* flush 2 bytes from input buffer */
    volatile uint8_t tmp;
    tmp = dev[uart]->DR;
    (void)tmp;
    tmp = dev[uart]->DR;
    (void)tmp;

    irq_restore(state);
}

void uart_poweroff(uart_t uart)
{
    unsigned state = irq_disable();

    /* disable and reset UART */
    dev[uart]->CSRB = 0;
    dev[uart]->CSRA = 0;

    switch (uart) {
        case 0: power_usart0_disable();
#if (UART_NUMOF == 2)
        case 1: power_usart1_disable();
#endif
#if (UART_NUMOF == 3)
        case 2: power_usart2_disable();
#endif
#if (UART_NUMOF == 4)
        /* workaround missing library function */
        case 3: PRR1 |= (1 << PRUSART3);
#endif
        default: return;
    }

    irq_restore(state);
}

static inline void isr_handler(int num)
{
    __enter_isr();

    /* check for valid byte */
    if (!((dev[num]->CSRA >> FE0) & 0x1)) {
        isr_ctx[num].rx_cb(isr_ctx[num].arg, dev[num]->DR);
    }

    __exit_isr();
}

#ifdef UART_0_ISR
ISR(UART_0_ISR, ISR_BLOCK)
{
    isr_handler(0);
}
#endif /* UART_0_ISR */

#ifdef UART_1_ISR
ISR(UART_1_ISR, ISR_BLOCK)
{
    isr_handler(1);
}
#endif /* UART_1_ISR */

#ifdef UART_2_ISR
ISR(UART_2_ISR, ISR_BLOCK)
{
    isr_handler(2);
}
#endif /* UART_2_ISR */

#ifdef UART_3_ISR
ISR(UART_3_ISR, ISR_BLOCK)
{
    isr_handler(3);
}
#endif /* UART_3_ISR */
