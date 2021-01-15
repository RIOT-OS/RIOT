/*
 * Copyright 2017 Ken Rabold
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_fe310
 * @{
 *
 * @file        rtt.c
 * @brief       Low-level RTT interface implementation for SiFive FE310
 *              The FE320's RTC module is what RIOT calls a Real-Time
 *              Timer (RTT), a simple counter which counts seconds; RIOT Real-
 *              Time Clocks (RTC) counts seconds, minutes, hours etc. There is
 *              an RTT->RTC wrapper layer in a separate file to allow using the
 *              RTT as a system real time clock.
 *
 * @author      Ken Rabold
 * @}
 */

#include <stdlib.h>
#include <unistd.h>

#include "cpu.h"
#include "periph_cpu.h"
#include "periph_conf.h"
#include "periph/rtt.h"
#include "plic.h"
#include "vendor/encoding.h"
#include "vendor/platform.h"
#include "vendor/plic_driver.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/* Convert RTT freq to pre-scaler value */
#ifdef RTT_FREQUENCY
#if ((RTT_CLOCK_FREQUENCY % RTT_FREQUENCY) != 0)
#error "Invalid RTT_FREQUENCY: Must be power of 2"
#endif
#endif
#define RTT_SCALE __builtin_ctz(RTT_CLOCK_FREQUENCY / RTT_FREQUENCY)

typedef struct {
    uint32_t alarm_val;     /**< cached alarm val */
    rtt_cb_t alarm_cb;      /**< callback called from RTC alarm */
    void *alarm_arg;        /**< argument passed to the callback */
    rtt_cb_t overflow_cb;   /**< callback called when RTC overflows */
    void *overflow_arg;     /**< argument passed to the callback */
}rtt_state_t;

static rtt_state_t rtt_callback;

void rtt_isr(int num)
{
    (void)num;

    /* Clear intr */
    AON_REG(AON_RTCCMP) = RTT_MAX_VALUE;

    /* Invoke callback function for alarm or overflow */
    if (rtt_callback.alarm_cb) {
        rtt_callback.alarm_cb(rtt_callback.alarm_arg);
    }
    else if (rtt_callback.overflow_cb) {
        rtt_callback.overflow_cb(rtt_callback.overflow_arg);
    }
}

void rtt_init(void)
{
    /* Disable ext interrupts when setting up */
    clear_csr(mie, MIP_MEIP);

    /* Configure RTC ISR with PLIC */
    plic_set_isr_cb(INT_RTCCMP, rtt_isr);
    plic_enable_interrupt(INT_RTCCMP);
    plic_set_priority(INT_RTCCMP, RTT_INTR_PRIORITY);

    /*  Configure RTC scaler, etc... */
    AON_REG(AON_RTCCFG) = RTT_SCALE;
    AON_REG(AON_RTCLO) = 0;
    AON_REG(AON_RTCHI) = 0;
    AON_REG(AON_RTCCMP) = RTT_MAX_VALUE;

    /* Configure state struct */
    rtt_callback.alarm_val = RTT_MAX_VALUE;
    rtt_callback.alarm_cb = NULL;
    rtt_callback.alarm_arg = NULL;
    rtt_callback.overflow_cb = NULL;
    rtt_callback.overflow_arg = NULL;

    /* Enable the RTC counter */
    rtt_poweron();

    /* Re-eanble ext interrupts */
    set_csr(mie, MIP_MEIP);
}

void rtt_set_overflow_cb(rtt_cb_t cb, void *arg)
{
    /*
     * No separate overflow intr
     * If no alarm cb is set, this cb will fire on RTC intr
     */
    rtt_callback.overflow_cb = cb;
    rtt_callback.overflow_arg = arg;
}

void rtt_clear_overflow_cb(void)
{
    /* No separate overflow intr */
    rtt_callback.overflow_cb = NULL;
    rtt_callback.overflow_arg = NULL;
}

uint32_t rtt_get_counter(void)
{
    /* Read scaled counter reg value */
    uint32_t t = AON_REG(AON_RTCS);

    return t;
}

void rtt_set_counter(uint32_t counter)
{
    /*
     * Can't write to scaled RTC reg
     * Must program HI/LO regs
     * Write scaled counter reg value
     */
    /* Use ifdef to avoid out of bound shift when RTT_SCALE == 0 */
#if RTT_CLOCK_FREQUENCY == RTT_FREQUENCY
    AON_REG(AON_RTCLO) = counter;
    AON_REG(AON_RTCHI) = 0;
#else
    AON_REG(AON_RTCLO) = counter << RTT_SCALE;
    AON_REG(AON_RTCHI) = counter >> (32 - RTT_SCALE);
#endif
}

void rtt_set_alarm(uint32_t alarm, rtt_cb_t cb, void *arg)
{
    /* Set cmp reg to given value */
    AON_REG(AON_RTCCMP) = alarm;
    rtt_callback.alarm_val = alarm;
    rtt_callback.alarm_cb = cb;
    rtt_callback.alarm_arg = arg;
}

uint32_t rtt_get_alarm(void)
{
    /*
     * Read back cached alarm value
     * When alarm fires, cmp reg is set to highest val
     * to clear intr, so used cached value for this function
     */
    return rtt_callback.alarm_val;
}

void rtt_clear_alarm(void)
{
    /* Set cmp reg to highest value */
    AON_REG(AON_RTCCMP) = RTT_MAX_VALUE;
    rtt_callback.alarm_val = RTT_MAX_VALUE;
    rtt_callback.alarm_cb = NULL;
    rtt_callback.alarm_arg = NULL;
}

void rtt_poweron(void)
{
    /* Enable the RTC counter */
    AON_REG(AON_RTCCFG) |= AON_RTCCFG_ENALWAYS;
}

void rtt_poweroff(void)
{
    /* Disable the RTC counter */
    AON_REG(AON_RTCCFG) &= ~AON_RTCCFG_ENALWAYS;
}
