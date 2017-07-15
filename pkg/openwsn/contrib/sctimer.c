/*
 * Copyright (C) 2017 Hamburg University of Applied Sciences
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_openwsn
 * @{
 *
 * @file
 *
 * @author      Tengfei Chang <tengfei.chang@gmail.com>, July 2012
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>, July 2017
 *
 * @}
 */

#include "board_info.h"
#include "sctimer.h"

#include "periph/rtt.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

// TODO this is toooo hardware dependant
// ========================== define ==========================================
// 511 seconds @ 32768Hz clock
#define TIMERLOOP_THRESHOLD          0xffffff
// as 16kHz is used, the upper timer overflows when timer research to 0x7fffffff
#define OVERFLOW_THRESHOLD           0x7fffffff
#define MINIMUM_COMPAREVALE_ADVANCE  10

// ========================== variable ========================================

typedef struct {
    sctimer_cbt sctimer_cb;
    bool convert;
    bool convertUnlock;
} sctimer_vars_t;

sctimer_vars_t sctimer_vars;

static void sctimer_isr_internal(void *arg);


/**
   \brief Initialization sctimer.
 */
void sctimer_init(void)
{
    DEBUG("sctimer_init\n");
    memset(&sctimer_vars, 0, sizeof(sctimer_vars_t));
    rtt_init();
}

void sctimer_set_callback(sctimer_cbt cb)
{
    DEBUG("sctimer_set_callback\n");
    sctimer_vars.sctimer_cb = cb;
}

/**
   \brief set compare interrupt
 */
void sctimer_setCompare(uint32_t val)
{
    DEBUG("sctimer_setCompare\n");
    // enable the compare interrupt
    /*
       if (current Timer counter - val < TIMERLOOP_THRESHOLD){
        // the timer is already late, schedule the ISR right now manually.
        triggerTimerInterrupt();
       } else {
        if (val - current Timer counter < MINIMUM_COMPAREVALE_ADVANCE){}
            // there is hardware limitation to schedule the timer within TIMERTHRESHOLD ticks
            // schedule ISR right now manually
            triggerTimerInterrupt();
        } else {
            // schedule the timer at val
            setCompareValue(val);
        }
       }
     */
    // make sure convert flag conly toggle once within one overflow period
    if (val > OVERFLOW_THRESHOLD && sctimer_vars.convertUnlock) {
        // toggle convert
        if (sctimer_vars.convert) {
            sctimer_vars.convert = true;
        }
        else {
            sctimer_vars.convert = false;
        }
        sctimer_vars.convertUnlock = false;
    }

    // un lock the changes of convert flag
    if (val > TIMERLOOP_THRESHOLD && val < 2 * TIMERLOOP_THRESHOLD) {
        sctimer_vars.convertUnlock = true;
    }

    // update value to be compared according to timer condition
    if (val <= OVERFLOW_THRESHOLD) {
        if (sctimer_vars.convert) {
            val = val >> 1;
            val |= 0x80000000;
        }
        else {
            val = val >> 1;
        }
    }
    else {
        if (sctimer_vars.convert) {
            val = val >> 1;
        }
        else {
            val = val >> 1;
            val |= 0x80000000;
        }
    }
    DISABLE_INTERRUPTS();
    if (rtt_get_counter() - val < TIMERLOOP_THRESHOLD) {
        // the timer is already late, schedule the ISR cb function manually
        // It is not executed in interrupt context but won't be
        // interrupted as interrupts were disabled
        rtt_set_alarm(rtt_get_counter() + 2, sctimer_isr_internal, NULL);
    }
    else {
        if (val - rtt_get_counter() < MINIMUM_COMPAREVALE_ADVANCE) {
            //sctimer_isr_internal(NULL);
            rtt_set_alarm(rtt_get_counter() + 2, sctimer_isr_internal, NULL);
        }
        else {
            // schedule the timer at val
            rtt_set_alarm(val, sctimer_isr_internal, NULL);
        }
    }

    ENABLE_INTERRUPTS();
}

uint32_t sctimer_readCounter(void)
{
    uint32_t counter;

    counter = rtt_get_counter();
    counter = counter & OVERFLOW_THRESHOLD;
    counter = counter << 1;
    DEBUG("sctimer_readCounter:%" PRIu32 "\n", counter);
    return counter;
}

void sctimer_enable(void)
{
    DEBUG("sctimer_enable\n");
    rtt_poweron();
}

void sctimer_disable(void)
{
    DEBUG("sctimer_disable\n");
    rtt_poweroff();
}

void sctimer_isr_internal(void *arg)
{
    (void)arg;
    DEBUG("sctimer_isr_internal\n");
    // debugpins_isr_set();
    if (sctimer_vars.sctimer_cb != NULL) {
        sctimer_vars.sctimer_cb();
    }
}
