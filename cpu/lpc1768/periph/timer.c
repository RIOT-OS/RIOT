/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_lpc1768
 * @{
 *
 * @file
 * @brief       Implementation of the low-level timer driver for the LPC1768
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @}
 */

#include <stdint.h>

#include "cpu.h"
#include "sched.h"
#include "thread.h"
#include "periph_conf.h"
#include "periph/timer.h"

/* guard file in case no timers are defined */
#if TIMER_0_EN

/**
 * @name Timer channel interrupt flags
 * @{
 */
#define MR0_FLAG    (0x01)      /**< match for channel 0 */
#define MR1_FLAG    (0x02)      /**< match for channel 1 */
#define MR2_FLAG    (0x04)      /**< match for channel 2 */
#define MR3_FLAG    (0x08)      /**< match for channel 3 */
/** @} */

/**
 * @brief Struct holding the configuration data for a UART device
 */
typedef struct {
    void (*cb)(int);            /**< timeout callback */
} timer_conf_t;

/**
 * @brief UART device configurations
 */
static timer_conf_t config[TIMER_NUMOF];

int timer_init(tim_t dev, unsigned int us_per_tick, void (*callback)(int))
{
    if (dev == TIMER_0) {
        /* save callback */
        config[TIMER_0].cb = callback;
        /* enable power for timer */
        TIMER_0_CLKEN();
        /* let timer run with full frequency */
        TIMER_0_PLKSEL();
        /* set to timer mode */
        TIMER_0_DEV->CTCR = 0;
        /* configure prescaler */
        TIMER_0_DEV->PR = (us_per_tick * TIMER_0_PRESCALER);
        /* configure and enable timer interrupts */
        NVIC_SetPriority(TIMER_0_IRQ, TIMER_IRQ_PRIO);
        NVIC_EnableIRQ(TIMER_0_IRQ);
        /* enable timer */
        TIMER_0_DEV->TCR |= 1;
        return 0;
    }
    return -1;
}

int timer_set(tim_t dev, int channel, unsigned int timeout)
{
    if (dev == TIMER_0) {
        unsigned int now = timer_read(dev);
        return timer_set_absolute(dev, channel, now + timeout);
    }
    return -1;
}

int timer_set_absolute(tim_t dev, int channel, unsigned int value)
{
    if (dev == TIMER_0) {
        switch (channel) {
            case 0:
                TIMER_0_DEV->MR0 = value;
                break;
            case 1:
                TIMER_0_DEV->MR1 = value;
                break;
            case 2:
                TIMER_0_DEV->MR2 = value;
                break;
            case 3:
                TIMER_0_DEV->MR3 = value;
                break;
            default:
                return -1;
        }
        TIMER_0_DEV->MCR |= (1 << (channel * 3));
        return 1;
    }
    return -1;
}

int timer_clear(tim_t dev, int channel)
{
    if (dev == TIMER_0 && channel >= 0 && channel < TIMER_0_CHANNELS) {
        TIMER_0_DEV->MCR &= ~(1 << (channel * 3));
        return 1;
    }
    return -1;
}

unsigned int timer_read(tim_t dev)
{
    if (dev == TIMER_0) {
        return (unsigned int)TIMER_0_DEV->TC;
    }
    return 0;
}

void timer_start(tim_t dev)
{
    if (dev == TIMER_0) {
        TIMER_0_DEV->TCR |= 1;
    }
}

void timer_stop(tim_t dev)
{
    if (dev == TIMER_0) {
        TIMER_0_DEV->TCR &= ~(1);
    }
}

void timer_irq_enable(tim_t dev)
{
    if (dev == TIMER_0) {
        NVIC_EnableIRQ(TIMER_0_IRQ);
    }
}

void timer_irq_disable(tim_t dev)
{
    if (dev == TIMER_0) {
        NVIC_DisableIRQ(TIMER_0_IRQ);
    }
}

void timer_reset(tim_t dev)
{
    if (dev == TIMER_0) {
        TIMER_0_DEV->TCR |= (1 << 1);
        asm("nop");                     /* just wait a cycle */
        TIMER_0_DEV->TCR &= ~(1 << 1);
    }
}

#if TIMER_0_EN
void TIMER_0_ISR(void)
{
    if (TIMER_0_DEV->IR & MR0_FLAG) {
        TIMER_0_DEV->IR |= (MR0_FLAG);
        TIMER_0_DEV->MCR &= ~(1 << 0);
        config[TIMER_0].cb(0);
    }
    if (TIMER_0_DEV->IR & MR1_FLAG) {
        TIMER_0_DEV->IR |= (MR1_FLAG);
        TIMER_0_DEV->MCR &= ~(1 << 3);
        config[TIMER_0].cb(1);
    }
    if (TIMER_0_DEV->IR & MR2_FLAG) {
        TIMER_0_DEV->IR |= (MR2_FLAG);
        TIMER_0_DEV->MCR &= ~(1 << 6);
        config[TIMER_0].cb(2);
    }
    if (TIMER_0_DEV->IR & MR3_FLAG) {
        TIMER_0_DEV->IR |= (MR3_FLAG);
        TIMER_0_DEV->MCR &= ~(1 << 9);
        config[TIMER_0].cb(3);
    }
    if (sched_context_switch_request) {
        thread_yield();
    }
}
#endif

#endif /* TIMER_0_EN */
