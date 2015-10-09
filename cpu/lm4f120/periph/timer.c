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

/* guard the file in case no TIMER is defined */
#if TIMER_NUMOF

/** Unified IRQ handler for all timers */
static inline void irq_handler(tim_t timer);

/**
 * @brief Struct holding the configuration data
 * @{
 */
typedef struct {
    void (*cb)(int);            /**< timeout callback */
} timer_conf_t;

/** Timer state memory */
static timer_conf_t config[TIMER_NUMOF];
/**@}*/

/**
 * The list of Timer peripherals.
 */
static const unsigned long g_ulTimerPeriph[2] =
{
    SYSCTL_PERIPH_WTIMER0,
    SYSCTL_PERIPH_WTIMER1
};

/**
 * The list of all possible base address of the timer
 */
static const unsigned long g_ulTimerBase[2] =
{
    WTIMER0_BASE,
    WTIMER1_BASE
};

/**
 * The list of possible interrupts for the Timer.
 */
static const unsigned long g_ulTimerInt[2] =
{
    INT_WTIMER0A,
    INT_WTIMER1A
};

int timer_init(tim_t dev, unsigned int us_per_tick, void (*callback)(int))
{
    ROM_SysCtlPeripheralEnable(g_ulTimerPeriph[dev]);           /* Activate Timer */
    switch (dev){
#if TIMER_0_EN
        case TIMER_0:
            WTIMER0_CTL_R &= ~0x00000001;                       /* Disable timer0A during setup */
            WTIMER0_CFG_R  = TIMER_CFG_16_BIT;
            WTIMER0_TAMR_R = TIMER_TAMR_TAMR_PERIOD;            /* Configure for periodic mode */
            WTIMER0_TAPR_R = TIMER_0_PRESCALER;                 /* 1us timer0A */
            WTIMER0_ICR_R  = 0x00000001;                        /* clear timer0A timeout flag */
            WTIMER0_IMR_R |= 0x00000001;                        /* arm timeout interrupt */
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            WTIMER1_CTL_R &= ~0x00000001;                       /* Disable timer1A during setup */
            WTIMER1_CFG_R  = TIMER_CFG_16_BIT;
            WTIMER1_TAMR_R = TIMER_TAMR_TAMR_PERIOD;            /* Configure for periodic mode */
            WTIMER1_TAPR_R = TIMER_1_PRESCALER;                 /* 1us timer1A */
            WTIMER1_ICR_R  = 0x00000001;                        /* clear timer1A timeout flag */
            WTIMER1_IMR_R |= 0x00000001;                        /* arm timeout interrupt */
            break;
#endif
        case TIMER_UNDEFINED:
        default:
            return -1;
    }

    /* set callback function */
    config[dev].cb = callback;

    /* set timer's IRQ priority */
    ROM_IntPrioritySet(g_ulTimerInt[dev], 32);

    /* enable the timer's interrupt */
    timer_irq_enable(dev);

    /* start the timer */
    timer_start(dev);

    DEBUG("startTimeout Value=0x%lx\n", ROM_TimerValueGet(g_ulTimerBase[dev], TIMER_A));

    return 0;
}

int timer_set(tim_t dev, int channel, unsigned int timeout)
{
    unsigned int now = timer_read(dev);
    DEBUG("timer_set now=0x%x\n",now);
    DEBUG("timer_set timeout=0x%x\n", timeout);
    return timer_set_absolute(dev, channel, now+timeout);
}

int timer_set_absolute(tim_t dev, int channel, unsigned int value)
{
    switch (dev){
#if TIMER_0_EN
        case TIMER_0:
            WTIMER0_TAILR_R = 0x00000000 | value;               /* period; Reload value */
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            WTIMER1_TAILR_R = 0x00000000 | value;               /* period; Reload value */
            break;
#endif
        case TIMER_UNDEFINED:
        default:
            return -1;
    }

    DEBUG("Setting timer absolute value=0x%x\n", value);
    return 0;
}

int timer_clear(tim_t dev, int channel)
{
    switch (dev){
#if TIMER_0_EN
        case TIMER_0:
            WTIMER0_ICR_R = TIMER_ICR_TATOCINT;
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            WTIMER1_ICR_R = TIMER_ICR_TATOCINT;
            break;
#endif
        case TIMER_UNDEFINED:
        default:
            return -1;
    }
    return -1;
}

unsigned int timer_read(tim_t dev)
{
    unsigned int currTimerVal=0;
    unsigned int loadTimerVal=0;
    switch(dev){
#if TIMER_0_EN
        case TIMER_0:
            currTimerVal = (unsigned int)ROM_TimerValueGet(g_ulTimerBase[dev], TIMER_A);
            loadTimerVal = (unsigned int)ROM_TimerLoadGet(g_ulTimerBase[dev], TIMER_A);
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            currTimerVal = (unsigned int)ROM_TimerValueGet(g_ulTimerBase[dev], TIMER_A);
            loadTimerVal = (unsigned int)ROM_TimerLoadGet(g_ulTimerBase[dev], TIMER_A);
            break;
#endif
        case TIMER_UNDEFINED:
        default:
            return 0;
    }
    DEBUG("Timer Read currTimerVal=0x%x\t loadTimerVal=0x%x\n", currTimerVal, loadTimerVal);
    return (loadTimerVal - currTimerVal);
}

void timer_start(tim_t dev)
{
    switch(dev){
#if TIMER_0_EN
        case TIMER_0:
            ROM_TimerEnable(g_ulTimerBase[dev], TIMER_A);
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            ROM_TimerEnable(g_ulTimerBase[dev], TIMER_A);
            break;
#endif
        case TIMER_UNDEFINED:
        default:
            break;
    }
}

void timer_stop(tim_t dev)
{
    switch (dev) {
#if TIMER_0_EN
        case TIMER_0:
            ROM_TimerDisable(g_ulTimerBase[dev], TIMER_A);
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            ROM_TimerDisable(g_ulTimerBase[dev], TIMER_A);
            break;
#endif
        case TIMER_UNDEFINED:
        default:
            break;
    }
}

void timer_irq_enable(tim_t dev)
{
    switch (dev) {
#if TIMER_0_EN
        case TIMER_0 :
            ROM_IntEnable(g_ulTimerInt[dev]);
            ROM_TimerIntEnable(g_ulTimerBase[dev], TIMER_TIMA_TIMEOUT);
            break;
#endif
#if TIMER_1_EN
        case TIMER_1 :
            ROM_IntEnable(g_ulTimerInt[dev]);
            ROM_TimerIntEnable(g_ulTimerBase[dev], TIMER_TIMA_TIMEOUT);
            break;
#endif
        case TIMER_UNDEFINED:
        default:
            break;
    }
}

void timer_irq_disable(tim_t dev)
{
    switch (dev) {
#if TIMER_0_EN
        case TIMER_0 :
            ROM_IntDisable(g_ulTimerInt[dev]);
            break;
#endif
#if TIMER_1_EN
        case TIMER_1 :
            ROM_IntDisable(g_ulTimerInt[dev]);
            break;
#endif
        case TIMER_UNDEFINED:
        default:
            break;
    }
}

void timer_reset(tim_t dev)
{
    switch (dev) {
#if TIMER_0_EN
        case TIMER_0 :
            /* Performs a software reset of a peripheral */
            ROM_SysCtlPeripheralReset(g_ulTimerPeriph[dev]);
            break;
#endif
#if TIMER_1_EN
        case TIMER_1 :
            /* Performs a software reset of a peripheral */
            ROM_SysCtlPeripheralReset(g_ulTimerPeriph[dev]);
            break;
#endif
        case TIMER_UNDEFINED:
        default:
            break;
    }
}

#if TIMER_0_EN
void TIMER_0_ISR(void)
{
    WTIMER0_ICR_R = TIMER_ICR_TATOCINT;
    irq_handler(TIMER_0);
}
#endif

#if TIMER_1_EN
void TIMER_1_ISR(void)
{
    WTIMER1_ICR_R = TIMER_ICR_TATOCINT;
    irq_handler(TIMER_1);
}
#endif

static inline void irq_handler(tim_t timer)
{
    config[timer].cb(0);
    if (sched_context_switch_request){
        thread_yield();
    }
}
#endif /*TIMER_NUMOF */
/** @} */
