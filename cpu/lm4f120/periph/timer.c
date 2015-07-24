/*
 * Copyright (C) 2015 Rakendra Thapa <rakendrathapa@gmail.com
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_lm4f120
 * @{
 *
 * @file        timer.c
 * @brief       Implementation of the low-level timer driver for the LM4F120
 *
 * @author      Rakendra Thapa <rakendrathapa@gmail.com>
 */

#include <stdint.h>

#include "cpu.h"
#include "sched.h"
#include "thread.h"
#include "periph_conf.h"
#include "periph/timer.h"
#include "mutex.h"

#define ENABLE_DEBUG (0)
#include "debug.h"
/* guard file in case no timers are defined */
#if TIMER_0_EN

/**
 * @brief Struct holding the configuration data
 * @{
 */
typedef struct {
    void (*cb)(int);            /**< timeout callback */
} timer_conf_t;

static timer_conf_t config[TIMER_NUMOF];
/**@}*/

int timer_init(tim_t dev, unsigned int us_per_tick, void (*callback)(int))
{
    if (dev == TIMER_0) {
        config[dev].cb = callback;                          /* User Function */
        ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_WTIMER0);  /* Activate Timer0 */
        WTIMER0_CTL_R &= ~0x00000001;                       /* Disable timer0A during setup */
        WTIMER0_CFG_R  = TIMER_CFG_16_BIT;
        WTIMER0_TAMR_R = TIMER_TAMR_TAMR_PERIOD;            /* Configure for periodic mode */
        WTIMER0_TAPR_R = TIMER_0_PRESCALER;                 /* 1us timer0A */
        WTIMER0_ICR_R  = 0x00000001;                        /* clear timer0A timeout flag */
        WTIMER0_IMR_R |= 0x00000001;                        /* arm timeout interrupt */
        ROM_IntPrioritySet(INT_WTIMER0A, 32);
        timer_irq_enable(dev);
        timer_start(dev);
        DEBUG("startTimeout Value=0x%lx\n", ROM_TimerValueGet(WTIMER0_BASE, TIMER_A));
        return 1;
    }
    return -1;
}

int timer_set(tim_t dev, int channel, unsigned int timeout)
{
    if (dev == TIMER_0) {
        unsigned int now = timer_read(dev);
        DEBUG("timer_set now=0x%x\n",now);
        DEBUG("timer_set timeout=0x%x\n", timeout);
        return timer_set_absolute(dev, channel, now+timeout);
    }
    return -1;
}

int timer_set_absolute(tim_t dev, int channel, unsigned int value)
{
    if (dev == TIMER_0) {
        WTIMER0_TAILR_R = 0x00000000 | value;               /* period; Reload value */
        DEBUG("Setting timer absolute value=0x%x\n", value);
        return 1;
    }
    return -1;
}

int timer_clear(tim_t dev, int channel)
{
    if (dev == TIMER_0){
        WTIMER0_ICR_R = TIMER_ICR_TATOCINT;
        return 1;
    }
    return -1;
}

unsigned int timer_read(tim_t dev)
{
    if (dev == TIMER_0) {
        unsigned int currTimer0Val=0;
        unsigned int loadTimer0Val=0;
        currTimer0Val = (unsigned int)ROM_TimerValueGet(WTIMER0_BASE, TIMER_A);
        loadTimer0Val = (unsigned int)ROM_TimerLoadGet(WTIMER0_BASE, TIMER_A);
        DEBUG("WTIMER0_TAILR_R=0x%lx\t currTimer0Val=0x%x\t loadTimer0Val=0x%x\n", WTIMER0_TAILR_R, currTimer0Val, loadTimer0Val);
        return (loadTimer0Val - currTimer0Val);
    }
    return 0;
}

void timer_start(tim_t dev)
{
    if (dev == TIMER_0) {
        ROM_TimerEnable(WTIMER0_BASE, TIMER_A);
    }
}

void timer_stop(tim_t dev)
{
    if (dev == TIMER_0) {
        ROM_TimerDisable(WTIMER0_BASE, TIMER_A);
    }
}

void timer_irq_enable(tim_t dev)
{
    if (dev == TIMER_0) {
        ROM_IntEnable(INT_WTIMER0A);
        ROM_TimerIntEnable(WTIMER0_BASE, TIMER_TIMA_TIMEOUT);
    }
}

void timer_irq_disable(tim_t dev)
{
    if (dev == TIMER_0) {
        ROM_IntDisable(INT_WTIMER0A);
    }
}

void timer_reset(tim_t dev)
{
    if (dev == TIMER_0) {
        /* Performs a software reset of a peripheral */
        ROM_SysCtlPeripheralReset(SYSCTL_PERIPH_WTIMER0);
    }
}

#if TIMER_0_EN
void isr_timer0a(void)
{
    TIMER0_ICR_R = TIMER_ICR_TATOCINT;
    config[TIMER_0].cb(0);

    if (sched_context_switch_request){
        thread_yield();
    }
}
void isr_wtimer0a(void)
{
    WTIMER0_ICR_R = TIMER_ICR_TATOCINT;

    config[TIMER_0].cb(0);
    if (sched_context_switch_request){
        thread_yield();
    }
}
#endif /* TIMER_0_EN */

#endif /* TIMER_0_EN */
/** @} */
