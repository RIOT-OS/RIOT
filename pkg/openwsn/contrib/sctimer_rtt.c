/*
 * Copyright (C) 2017 Hamburg University of Applied Sciences
 *               2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_openwsn
 * @{
 *
 * For details on the implementation check pkg/openwsn/doc.txt
 *
 * @file
 * @brief       RTT based adaptation of "sctimer" bsp module
 *
 * @author      Tengfei Chang <tengfei.chang@gmail.com>, July 2012
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>, July 2017
 * @author      Michel Rottleuthner <michel.rottleuthner@haw-hamburg.de>, April 2019
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 * @}
 */
#include <stdatomic.h>

#include "sctimer.h"
#include "debugpins.h"

#include "board.h"
#include "periph/rtt.h"

#define LOG_LEVEL LOG_NONE
#include "log.h"

/**
 * @brief   Maximum counter difference to not consider an ISR late, this
 *          should account for the largest timer interval OpenWSN
 *          scheduler might work with. When running only the stack this
 *          should not be more than SLOT_DURATION, but when using cjoin
 *          it is 65535ms
 */
#ifndef SCTIMER_LOOP_THRESHOLD
#define SCTIMER_LOOP_THRESHOLD      (2 * PORT_TICS_PER_MS * 65535)
#endif

/* OpenWSN needs at least 32 tics per ms,use time division to reach that
   if needed */
#ifdef RTT_FREQUENCY
#if RTT_FREQUENCY < 32768U
#define SCTIMER_TIME_DIVISION       (1)
#if (SCTIMER_FREQUENCY % RTT_FREQUENCY) != 0
#error "RTT_FREQUENCY not supported"
#endif
#endif
#endif

#ifdef SCTIMER_TIME_DIVISION
#define SCTIMER_PRESCALER            __builtin_ctz( \
        SCTIMER_FREQUENCY / RTT_FREQUENCY)
#define SCTIMER_TIME_DIVISION_MASK   (RTT_MAX_VALUE >> SCTIMER_PRESCALER)
#define SCTIMER_PRESCALER_MASK       (~SCTIMER_TIME_DIVISION_MASK)
#define SCTIMER_PRESCALER_SHIFT      __builtin_ctz(SCTIMER_TIME_DIVISION_MASK)

static uint32_t _prescaler;
static atomic_bool _enable;
#endif

#if RTT_MAX_VALUE < UINT32_MAX
/* If RTT_MAX_VALUE is smaller the UINT32_MAX then handle the remaining
   bits here, sctimer is scheduled at least every slot (20ms) so no
   overflow will be missed */
#define SCTIMER_RTT_EXTEND_MSB  (1 << (32UL - __builtin_clz(RTT_MAX_VALUE)))
static atomic_uint_fast32_t _counter_msb;
static atomic_uint_fast32_t _now_last;
#endif

static sctimer_cbt sctimer_cb;

static void sctimer_isr_internal(void *arg)
{
    (void)arg;

    if (sctimer_cb != NULL) {
        debugpins_isr_set();
        sctimer_cb();
        debugpins_isr_clr();
    }
}

void sctimer_init(void)
{
    rtt_init();
    sctimer_cb = NULL;
#ifdef SCTIMER_RTT_EXTEND_MSB
    atomic_store(&_counter_msb, 0);
    atomic_store(&_now_last, 0);
#endif
#ifdef SCTIMER_TIME_DIVISION
    _prescaler = 0;
    _enable = false;
#endif
}

void sctimer_set_callback(sctimer_cbt cb)
{
    sctimer_cb = cb;
}

#ifdef SCTIMER_TIME_DIVISION
uint32_t _update_val(uint32_t val, uint32_t now)
{
    now = now & SCTIMER_PRESCALER_MASK;
    val = val >> SCTIMER_PRESCALER;
    /* Check if next value would cause an overflow */
    if ((now - val) > SCTIMER_LOOP_THRESHOLD && _enable && now > val) {
        _prescaler += (1 << SCTIMER_PRESCALER_SHIFT);
        _enable = false;
    }
    /* Make sure it only updates the _prescaler once per overflow cycle */
    if (val > SCTIMER_LOOP_THRESHOLD && val < 2 * SCTIMER_LOOP_THRESHOLD) {
        _enable = true;
    }
    val |= _prescaler;

    return val;
}
#endif

#ifdef SCTIMER_RTT_EXTEND_MSB
uint32_t _sctimer_extend(uint32_t now)
{
    unsigned state = irq_disable();
    uint32_t now_last = atomic_load(&_now_last);
    uint32_t counter_msb = atomic_load(&_counter_msb);
    if (now < now_last) {
        /* account for overflow */
        counter_msb += SCTIMER_RTT_EXTEND_MSB;
        atomic_store(&_counter_msb, counter_msb);
    }
    atomic_store(&_now_last, now);
    now += counter_msb;
    irq_restore(state);
    return now;
}
#endif

void sctimer_setCompare(uint32_t val)
{
    unsigned state = irq_disable();

    uint32_t now = rtt_get_counter();

#ifdef SCTIMER_RTT_EXTEND_MSB
    now = _sctimer_extend(now);
#endif

#ifdef SCTIMER_TIME_DIVISION
    val = _update_val(val, now);
#endif

    if ((int32_t)now - val < SCTIMER_LOOP_THRESHOLD && now > val) {
        rtt_set_alarm((now + RTT_MIN_OFFSET) & RTT_MAX_VALUE,
                      sctimer_isr_internal, NULL);
    }
    else {
        if ((int32_t)val - now < RTT_MIN_OFFSET) {
            rtt_set_alarm((now + RTT_MIN_OFFSET) & RTT_MAX_VALUE,
                          sctimer_isr_internal, NULL);
        }
        else {
            rtt_set_alarm(val & RTT_MAX_VALUE, sctimer_isr_internal,
                          NULL);
        }
    }

    irq_restore(state);

    LOG_DEBUG("[sctimer]: set cb to %" PRIu32 " at %" PRIu32 "\n",
              (uint32_t) val, now);
}

uint32_t sctimer_readCounter(void)
{
    uint32_t now = rtt_get_counter();

#ifdef SCTIMER_RTT_EXTEND_MSB
    now = _sctimer_extend(now);
#endif

#ifdef SCTIMER_TIME_DIVISION
    now &= SCTIMER_TIME_DIVISION_MASK;
    now = (now << SCTIMER_PRESCALER);
#endif
    LOG_DEBUG("[sctimer]: now %" PRIu32 "\n", now);
    return now;
}

void sctimer_enable(void)
{
    rtt_poweron();
}

void sctimer_disable(void)
{
    rtt_poweroff();
}
