/*
 * Copyright (C) 2015 Attilio Dona'
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_cc3200
 * @{
 *
 * @file
 * @brief       Low-level timer driver implementation
 *
 * @author      Attilio Dona'
 *
 * @}
 */

#include <stdlib.h>

#include <thread.h>
#include "panic.h"
#include <sys/types.h>

#include "periph_conf.h"
#include "periph/timer.h"

#include "driverlib/timer.h"
#include "driverlib/prcm.h"

#include <inc/hw_memmap.h>
#include <inc/hw_timer.h>
#include <inc/hw_types.h>

#define MAX_TIMERS TIMER_UNDEFINED

typedef struct {
    void (*cb)(void*, int);
} timer_conf_t;

static timer_conf_t config[MAX_TIMERS];


void irq_timer0_handler(void) {
    timer_clear(TIMER_0, 0);
    config[TIMER_0].cb(0,0); // timer has one hw channel
    if (sched_context_switch_request) {
        thread_yield();
    }
}

void irq_timer1_handler(void) {
    timer_clear(TIMER_1, 0);
    config[TIMER_1].cb(0,0); // timer has one hw channel
    if (sched_context_switch_request) {
        thread_yield();
    }
}

void irq_timer2_handler(void) {
    timer_clear(TIMER_2, 0);
    config[TIMER_2].cb(0,0); // timer has one hw channel
    if (sched_context_switch_request) {
        thread_yield();
    }
}

void irq_timer3_handler(void) {
    timer_clear(TIMER_3, 0);
    config[TIMER_3].cb(0,0); // timer has one hw channel
    if (sched_context_switch_request) {
        thread_yield();
    }
}


int timer_init(tim_t dev, unsigned long freq, timer_cb_t cb, void *arg) {

    switch (dev) {
    case TIMER_0:
        //
        // Enable and Reset the timer block
        //
        MAP_PRCMPeripheralClkEnable(PRCM_TIMERA0, PRCM_RUN_MODE_CLK);
        MAP_PRCMPeripheralReset(PRCM_TIMERA0);

        MAP_TimerConfigure(TIMERA0_BASE, TIMER_CFG_PERIODIC_UP);

        MAP_TimerControlStall(TIMERA0_BASE, TIMER_A, true);
        // register the handler
        MAP_TimerIntRegister(TIMERA0_BASE, TIMER_A, irq_timer0_handler);

        config[TIMER_0].cb = cb;

        MAP_IntPrioritySet(INT_TIMERA0A, INT_PRIORITY_LVL_2);

        // enable the timer
        MAP_TimerEnable(TIMERA0_BASE, TIMER_A);

        break;
    case TIMER_1:
        //
        // Enable and Reset the timer block
        //
        MAP_PRCMPeripheralClkEnable(PRCM_TIMERA1, PRCM_RUN_MODE_CLK);
        MAP_PRCMPeripheralReset(PRCM_TIMERA1);

        MAP_TimerConfigure(TIMERA1_BASE, TIMER_CFG_PERIODIC_UP);

        MAP_TimerControlStall(TIMERA1_BASE, TIMER_A, true);

        // register the handler
        MAP_TimerIntRegister(TIMERA1_BASE, TIMER_A, irq_timer1_handler);

        config[TIMER_1].cb = cb;

        MAP_IntPrioritySet(INT_TIMERA1A, INT_PRIORITY_LVL_2);

        // enable the timer
        MAP_TimerEnable(TIMERA1_BASE, TIMER_A);

        break;
    case TIMER_2:
        //
        // Enable and Reset the timer block
        //
        MAP_PRCMPeripheralClkEnable(PRCM_TIMERA2, PRCM_RUN_MODE_CLK);
        MAP_PRCMPeripheralReset(PRCM_TIMERA2);

        MAP_TimerConfigure(TIMERA2_BASE, TIMER_CFG_PERIODIC_UP);

        MAP_TimerControlStall(TIMERA2_BASE, TIMER_A, true);

        // register the handler
        MAP_TimerIntRegister(TIMERA2_BASE, TIMER_A, irq_timer2_handler);

        config[TIMER_2].cb = cb;

        MAP_IntPrioritySet(INT_TIMERA2A, INT_PRIORITY_LVL_2);

        // enable the timer
        MAP_TimerEnable(TIMERA2_BASE, TIMER_A);

        break;
    case TIMER_3:
        //
        // Enable and Reset the timer block
        //
        MAP_PRCMPeripheralClkEnable(PRCM_TIMERA3, PRCM_RUN_MODE_CLK);
        MAP_PRCMPeripheralReset(PRCM_TIMERA3);

        MAP_TimerConfigure(TIMERA3_BASE, TIMER_CFG_PERIODIC_UP);

        MAP_TimerControlStall(TIMERA3_BASE, TIMER_A, true);

        // register the handler
        MAP_TimerIntRegister(TIMERA3_BASE, TIMER_A, irq_timer3_handler);

        config[TIMER_3].cb = cb;

        MAP_IntPrioritySet(INT_TIMERA3A, INT_PRIORITY_LVL_2);

        // enable the timer
        MAP_TimerEnable(TIMERA3_BASE, TIMER_A);

        break;
    default:
        break;
    }

    return 0;
}

int set_absolute(tim_t dev, int channel, unsigned long long value) {
    switch (dev) {
        case TIMER_0:
            MAP_TimerMatchSet(TIMERA0_BASE, TIMER_A, value);
            // enable the match timer
            HWREG(TIMERA0_BASE + TIMER_O_IMR) |= TIMER_TIMA_MATCH;
            break;
        case TIMER_1:
            MAP_TimerMatchSet(TIMERA1_BASE, TIMER_A, value);
            // enable the match timer
            HWREG(TIMERA1_BASE + TIMER_O_IMR) |= TIMER_TIMA_MATCH;
            break;
        case TIMER_2:
            MAP_TimerMatchSet(TIMERA2_BASE, TIMER_A, value);
            // enable the match timer
            HWREG(TIMERA2_BASE + TIMER_O_IMR) |= TIMER_TIMA_MATCH;
            break;
        case TIMER_3:
            MAP_TimerMatchSet(TIMERA3_BASE, TIMER_A, value);
            // enable the match timer
            HWREG(TIMERA3_BASE + TIMER_O_IMR) |= TIMER_TIMA_MATCH;
            break;
        default:
            break;
    }

    return 0;

}

int timer_set(tim_t dev, int channel, unsigned int timeout) {

    switch (dev) {
        case TIMER_0:
            return set_absolute(dev, channel,
                                HWREG(TIMERA0_BASE + TIMER_O_TAR) + timeout);
        case TIMER_1:
            return set_absolute(dev, channel,
                                HWREG(TIMERA1_BASE + TIMER_O_TAR) + timeout);
        case TIMER_2:
            return set_absolute(dev, channel,
                                HWREG(TIMERA2_BASE + TIMER_O_TAR) + timeout);
        case TIMER_3:
            return set_absolute(dev, channel,
                                HWREG(TIMERA3_BASE + TIMER_O_TAR) + timeout);
        default:
            break;
    }

    return 0;
}

int timer_set_absolute(tim_t dev, int channel, unsigned int value) {
    return set_absolute(dev, channel, value);
}

int timer_clear(tim_t dev, int channel) {
    switch (dev) {
    case TIMER_0:
        MAP_TimerIntClear(TIMERA0_BASE, TIMER_TIMA_MATCH);
        // disable the match timer
        HWREG(TIMERA0_BASE + TIMER_O_IMR) &= ~(TIMER_TIMA_MATCH);
        break;
    case TIMER_1:
        MAP_TimerIntClear(TIMERA1_BASE, TIMER_TIMA_MATCH);
        // disable the match timer
        HWREG(TIMERA1_BASE + TIMER_O_IMR) &= ~(TIMER_TIMA_MATCH);
        break;
    case TIMER_2:
        MAP_TimerIntClear(TIMERA2_BASE, TIMER_TIMA_MATCH);
        // disable the match timer
        HWREG(TIMERA2_BASE + TIMER_O_IMR) &= ~(TIMER_TIMA_MATCH);
        break;
    case TIMER_3:
        MAP_TimerIntClear(TIMERA3_BASE, TIMER_TIMA_MATCH);
        // disable the match timer
        HWREG(TIMERA3_BASE + TIMER_O_IMR) &= ~(TIMER_TIMA_MATCH);
        break;
    default:
        break;
    }
    return 0;
}

unsigned int timer_read(tim_t dev) {
    switch (dev) {
    case TIMER_0:
        return MAP_TimerValueGet(TIMERA0_BASE, TIMER_A);
        break;
    case TIMER_1:
        return MAP_TimerValueGet(TIMERA1_BASE, TIMER_A);
        break;
    case TIMER_2:
        return MAP_TimerValueGet(TIMERA2_BASE, TIMER_A);
        break;
    case TIMER_3:
        return MAP_TimerValueGet(TIMERA3_BASE, TIMER_A);
        break;
    default:
        return 0;
    }
}

void timer_start(tim_t dev) {
    switch (dev) {
    case TIMER_0:
        MAP_TimerEnable(TIMERA0_BASE, TIMER_A);
        break;
    case TIMER_1:
        MAP_TimerEnable(TIMERA1_BASE, TIMER_A);
        break;
    case TIMER_2:
        MAP_TimerEnable(TIMERA2_BASE, TIMER_A);
        break;
    case TIMER_3:
        MAP_TimerEnable(TIMERA3_BASE, TIMER_A);
        break;
    default:
        break;
    }
}

void timer_stop(tim_t dev) {
    switch (dev) {
    case TIMER_0:
        MAP_TimerDisable(TIMERA0_BASE, TIMER_A);
        break;
    case TIMER_1:
        MAP_TimerDisable(TIMERA1_BASE, TIMER_A);
        break;
    case TIMER_2:
        MAP_TimerDisable(TIMERA2_BASE, TIMER_A);
        break;
    case TIMER_3:
        MAP_TimerDisable(TIMERA3_BASE, TIMER_A);
        break;
    default:
        break;
    }
}

void timer_irq_enable(tim_t dev) {
    switch (dev) {
    case TIMER_0:
        MAP_TimerIntEnable(TIMERA0_BASE, TIMER_TIMA_MATCH);
        break;
    case TIMER_1:
        MAP_TimerIntEnable(TIMERA1_BASE, TIMER_TIMA_MATCH);
        break;
    case TIMER_2:
        MAP_TimerIntEnable(TIMERA2_BASE, TIMER_TIMA_MATCH);
        break;
    case TIMER_3:
        MAP_TimerIntEnable(TIMERA3_BASE, TIMER_TIMA_MATCH);
        break;
    default:
        break;
    }
}

void timer_irq_disable(tim_t dev) {
    switch (dev) {
    case TIMER_0:
        MAP_TimerIntDisable(TIMERA0_BASE, TIMER_TIMA_MATCH);
        break;
    case TIMER_1:
        MAP_TimerIntDisable(TIMERA1_BASE, TIMER_TIMA_MATCH);
        break;
    case TIMER_2:
        MAP_TimerIntDisable(TIMERA2_BASE, TIMER_TIMA_MATCH);
        break;
    case TIMER_3:
        MAP_TimerIntDisable(TIMERA3_BASE, TIMER_TIMA_MATCH);
        break;
    default:
        break;
    }
}

void timer_reset(tim_t dev) {
    switch (dev) {
    case TIMER_0:
        TimerValueSet(TIMERA0_BASE, TIMER_A, 0);
        break;
    case TIMER_1:
        TimerValueSet(TIMERA1_BASE, TIMER_A, 0);
        break;
    case TIMER_2:
        TimerValueSet(TIMERA2_BASE, TIMER_A, 0);
        break;
    case TIMER_3:
        TimerValueSet(TIMERA3_BASE, TIMER_A, 0);
        break;
    default:
        break;
    }
}
