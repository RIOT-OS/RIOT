/*
 * Copyright (C) 2018 Acutam Automation, LLC
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_periph_uart
 * @{
 *
 * @file
 * @brief       common UART function fallback implementations
 *
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 *
 * @}
 */

#include "irq.h"
#include "periph/gpio.h"
#include "periph/uart.h"
#include "xtimer.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#ifdef UART_NUMOF

#ifdef PERIPH_UART_NEEDS_BREAK_SLEEP

#define UART_DEBOUNCE_DELAY (10000U)    /* 10ms */

static void _break_sleep(uart_t uart, void *arg);
static void _break_wake(void *arg);

static inline unsigned _uart_revlookup(uart_t uart)
{
    for (unsigned i = 0; i < UART_NUMOF; i++) {
        if (UART_DEV(i) == uart) {
            return i;
        }
    }

    /* return error */
    return (unsigned)(-1);
}

int uart_break_sleep_init(uart_t uart)
{
    /* make sure the device is valid */
    if (uart >= UART_NUMOF) {
        return UART_NODEV;
    }
    unsigned uartnum = _uart_revlookup(uart);
    if (uartnum == (unsigned)(-1)) {
        return UART_NODEV;
    }

    unsigned state = irq_disable();

    gpio_t pin = UART_WAKE_PINS(uartnum);

    /* register wake callback */
    if (gpio_init_int(pin, GPIO_IN_PD, GPIO_RISING,
                      _break_wake, (void *)(uintptr_t)uartnum) < 0) {

        /* built-in pulldown not supported */
        if (gpio_init_int(pin, GPIO_IN, GPIO_RISING,
                          _break_wake, (void *)(uintptr_t)uartnum) < 0) {
            irq_restore(state);
            DEBUG("[uart] gpio_init_int failed on %u for UART%i\n",
                  (unsigned)pin, uartnum);
            return UART_INTERR;
        }
    }

    /* register sleep callback */
    if (uart_break_init(_break_sleep, 0x0) != UART_OK) {
        irq_restore(state);

        DEBUG("[uart] uart_break_init failed for UART%i\n", uartnum);
        return UART_INTERR;
    }

    /* set initial wake state */
    if (!gpio_read(pin)) {
        DEBUG("[uart] starting asleep on UART%i\n", uartnum);

        uart_poweroff(uart);
        gpio_irq_enable(pin);
    }
    else {
        gpio_irq_disable(pin);
    }

    irq_restore(state);

    DEBUG("[uart] UART%i now set to sleep on break condition\n", uartnum);

    return UART_OK;
}

void uart_break_sleep_enable(void)
{
    uart_break_init(_break_sleep, 0x0);
}

void uart_break_sleep_disable(void)
{
    uart_break_init(NULL, 0x0);
}

static void _break_sleep(uart_t uart, void *arg)
{
    (void)arg;

    unsigned uartnum = _uart_revlookup(uart);

    if (UART_WAKE_PINS(uartnum) != GPIO_UNDEF) {
        DEBUG("[uart] UART%i going to sleep\n", uartnum);

        /* wait for contact bounce to quiet */
        xtimer_usleep(UART_DEBOUNCE_DELAY);

        uart_poweroff(uart);
        gpio_irq_enable(UART_WAKE_PINS(uartnum));
    }
}

static void _break_wake(void *arg)
{
    unsigned uartnum = (unsigned)(uintptr_t)arg;

    gpio_irq_disable(UART_WAKE_PINS(uartnum));

    /* wait for contact bounce to quiet */
    xtimer_usleep(UART_DEBOUNCE_DELAY);

    uart_poweron(UART_DEV(uartnum));

    DEBUG("[uart] UART%i waking up\n", uartnum);
}

#endif /* PERIPH_UART_NEEDS_BREAK_SLEEP */

#endif /* UART_NUMOF */
