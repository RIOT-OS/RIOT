/*
 * SPDX-FileCopyrightText: 2017 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     pkg_micropython
 * @{
 *
 * @file
 * @brief       MicroPython HAL port definitions for RIOT
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <time.h>

#include "xtimer.h"

#define MP_HAL_PIN_FMT  "%u"
#define mp_hal_pin_name(p) (p)

static inline mp_uint_t mp_hal_ticks_ms(void)
{
    return xtimer_now_usec() / 1000;
}

static inline mp_uint_t mp_hal_ticks_us(void)
{
    return xtimer_now_usec();
}

/* Not inline: this drains MicroPython's scheduler queue while sleeping, so
 * deferred callbacks (e.g. xtimer) run during time.sleep()/sleep_ms(). See
 * mphalport.c. */
void mp_hal_delay_ms(mp_uint_t ms);

static inline void mp_hal_delay_us(mp_uint_t us)
{
    /* Short, precise delay used by drivers for timing; intentionally does not
     * run scheduled callbacks (sleep_us() maps here). */
    return xtimer_usleep(us);
}

static inline mp_uint_t mp_hal_ticks_cpu(void)
{
    return xtimer_now_usec();
}

static inline uint64_t mp_hal_time_ns(void)
{
    return (uint64_t)time(NULL) * 1000000000ULL;
}

static inline void mp_hal_set_interrupt_char(char c)
{
}
