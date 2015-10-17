/*
 * Copyright (C) 2014 Loci Controls Inc.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_periph
 * @{
 *
 * @file
 * @brief       Low-level timer driver implementation for the CC2538 CPU
 *
 * @author      Ian Martin <ian@locicontrols.com>
 *
 * @}
 */

#include <stdlib.h>
#include <stdio.h>

#include "board.h"
#include "cpu.h"
#include "sched.h"
#include "thread.h"
#include "periph/timer.h"
#include "periph_conf.h"

#define USEC_PER_SEC             1000000 /**< Conversion factor between seconds and microseconds */

typedef struct {
    void (*cb)(int);
} timer_conf_t;

/**
 * @brief Timer state memory
 */
timer_conf_t config[TIMER_NUMOF];


/**
 * @brief Setup the given timer
 *
 */
int timer_init(tim_t dev, unsigned int ticks_per_us, void (*callback)(int))
{
    cc2538_gptimer_t *gptimer;
    unsigned int gptimer_num;

    /* select the timer and enable the timer specific peripheral clocks */
    switch (dev) {
#if TIMER_0_EN
        case TIMER_0:
            gptimer = TIMER_0_DEV;
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            gptimer = TIMER_1_DEV;
            break;
#endif
#if TIMER_2_EN
        case TIMER_2:
            gptimer = TIMER_2_DEV;
            break;
#endif
#if TIMER_3_EN
        case TIMER_3:
            gptimer = TIMER_3_DEV;
            break;
#endif

        case TIMER_UNDEFINED:
        default:
            return -1;
    }

    gptimer_num = ((uintptr_t)gptimer - (uintptr_t)GPTIMER0) / 0x1000;

    /* Save the callback function: */
    config[dev].cb = callback;

    /* Enable the clock for this timer: */
    SYS_CTRL_RCGCGPT |= (1 << gptimer_num);

    /* Disable this timer before configuring it: */
    gptimer->CTL = 0;

    gptimer->CFG  = GPTMCFG_16_BIT_TIMER;
    gptimer->TAMR = GPTIMER_PERIODIC_MODE;
    gptimer->TAMRbits.TACDIR = 1; /**< Count up */

    /* Set the prescale register for the desired frequency: */
    gptimer->TAPR = RCOSC16M_FREQ / (ticks_per_us * USEC_PER_SEC) - 1;

    /* Enable interrupts for given timer: */
    timer_irq_enable(dev);

    /* Enable the timer: */
    gptimer->CTLbits.TAEN = 1;

    return 0;
}

int timer_set(tim_t dev, int channel, unsigned int timeout)
{
    return timer_set_absolute(dev, channel, timer_read(dev) + timeout);
}

int timer_set_absolute(tim_t dev, int channel, unsigned int value)
{
    cc2538_gptimer_t *gptimer;

    /* get timer base register address */
    switch (dev) {
#if TIMER_0_EN
        case TIMER_0:
            gptimer = TIMER_0_DEV;
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            gptimer = TIMER_1_DEV;
            break;
#endif
#if TIMER_2_EN
        case TIMER_2:
            gptimer = TIMER_2_DEV;
            break;
#endif
#if TIMER_3_EN
        case TIMER_3:
            gptimer = TIMER_3_DEV;
            break;
#endif
        case TIMER_UNDEFINED:
        default:
            return -1;
    }

    /* set timeout value */
    switch (channel) {
        case 0:
            gptimer->TAILR = value;
            break;

        case 1:
            gptimer->TBILR = value;
            break;

        default:
            return -1;
    }

    return 1;
}

int timer_clear(tim_t dev, int channel)
{
    cc2538_gptimer_t *gptimer;

    /* get timer base register address */
    switch (dev) {
#if TIMER_0_EN
        case TIMER_0:
            gptimer = TIMER_0_DEV;
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            gptimer = TIMER_1_DEV;
            break;
#endif
#if TIMER_2_EN
        case TIMER_2:
            gptimer = TIMER_2_DEV;
            break;
#endif
#if TIMER_3_EN
        case TIMER_3:
            gptimer = TIMER_3_DEV;
            break;
#endif

        case TIMER_UNDEFINED:
        default:
            return -1;
    }

    switch (channel) {
        case 0:
            gptimer->CTLbits.TAEN = 0;
            break;

        case 1:
            gptimer->CTLbits.TBEN = 0;
            break;

        default:
            return -1;
    }

    return 1;
}

/*
 * The timer channels 1 and 2 are configured to run with the same speed and
 * have the same value (they run in parallel), so only on of them is returned.
 */
unsigned int timer_read(tim_t dev)
{
    switch (dev) {
#if TIMER_0_EN
        case TIMER_0:
            return TIMER_0_DEV->TAR;
#endif
#if TIMER_1_EN
        case TIMER_1:
            return TIMER_1_DEV->TAR;
#endif
#if TIMER_2_EN
        case TIMER_2:
            return TIMER_2_DEV->TAR;
#endif
#if TIMER_3_EN
        case TIMER_3:
            return TIMER_3_DEV->TAR;
#endif

        case TIMER_UNDEFINED:
        default:
            return 0;
    }
}

/*
 * For stopping the counting of all channels.
 */
void timer_stop(tim_t dev)
{
    switch (dev) {
#if TIMER_0_EN
        case TIMER_0:
            TIMER_0_DEV->CTLbits.TAEN = 0;
            TIMER_0_DEV->CTLbits.TBEN = 0;
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            TIMER_1_DEV->CTLbits.TAEN = 0;
            TIMER_1_DEV->CTLbits.TBEN = 0;
            break;
#endif
#if TIMER_2_EN
        case TIMER_2:
            TIMER_2_DEV->CTLbits.TAEN = 0;
            TIMER_2_DEV->CTLbits.TBEN = 0;
            break;
#endif
#if TIMER_3_EN
        case TIMER_3:
            TIMER_3_DEV->CTLbits.TAEN = 0;
            TIMER_3_DEV->CTLbits.TBEN = 0;
            break;
#endif

        case TIMER_UNDEFINED:
            break;
    }
}

void timer_start(tim_t dev)
{
    switch (dev) {
#if TIMER_0_EN
        case TIMER_0:
            TIMER_0_DEV->CTLbits.TAEN = 1;
            TIMER_0_DEV->CTLbits.TBEN = 1;
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            TIMER_1_DEV->CTLbits.TAEN = 1;
            TIMER_1_DEV->CTLbits.TBEN = 1;
            break;
#endif
#if TIMER_2_EN
        case TIMER_2:
            TIMER_2_DEV->CTLbits.TAEN = 1;
            TIMER_2_DEV->CTLbits.TBEN = 1;
            break;
#endif
#if TIMER_3_EN
        case TIMER_3:
            TIMER_3_DEV->CTLbits.TAEN = 1;
            TIMER_3_DEV->CTLbits.TBEN = 1;
            break;
#endif

        case TIMER_UNDEFINED:
            break;
    }
}

void timer_irq_enable(tim_t dev)
{
    switch (dev) {
#if TIMER_0_EN
        case TIMER_0:
            NVIC_SetPriority(TIMER_0_IRQn_1, TIMER_IRQ_PRIO);
            NVIC_SetPriority(TIMER_0_IRQn_2, TIMER_IRQ_PRIO);
            NVIC_EnableIRQ(TIMER_0_IRQn_1);
            NVIC_EnableIRQ(TIMER_0_IRQn_2);
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            NVIC_SetPriority(TIMER_1_IRQn_1, TIMER_IRQ_PRIO);
            NVIC_SetPriority(TIMER_1_IRQn_2, TIMER_IRQ_PRIO);
            NVIC_EnableIRQ(TIMER_1_IRQn_1);
            NVIC_EnableIRQ(TIMER_1_IRQn_2);
            break;
#endif
#if TIMER_2_EN
        case TIMER_2:
            NVIC_SetPriority(TIMER_2_IRQn_1, TIMER_IRQ_PRIO);
            NVIC_SetPriority(TIMER_2_IRQn_2, TIMER_IRQ_PRIO);
            NVIC_EnableIRQ(TIMER_2_IRQn_1);
            NVIC_EnableIRQ(TIMER_2_IRQn_2);
            break;
#endif
#if TIMER_3_EN
        case TIMER_3:
            NVIC_SetPriority(TIMER_3_IRQn_1, TIMER_IRQ_PRIO);
            NVIC_SetPriority(TIMER_3_IRQn_2, TIMER_IRQ_PRIO);
            NVIC_EnableIRQ(TIMER_3_IRQn_1);
            NVIC_EnableIRQ(TIMER_3_IRQn_2);
            break;
#endif

        case TIMER_UNDEFINED:
        default:
            return;
    }
}

void timer_irq_disable(tim_t dev)
{
    switch (dev) {
#if TIMER_0_EN
        case TIMER_0:
            NVIC_DisableIRQ(TIMER_0_IRQn_1);
            NVIC_DisableIRQ(TIMER_0_IRQn_2);
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            NVIC_DisableIRQ(TIMER_1_IRQn_1);
            NVIC_DisableIRQ(TIMER_1_IRQn_2);
            break;
#endif
#if TIMER_2_EN
        case TIMER_2:
            NVIC_DisableIRQ(TIMER_2_IRQn_1);
            NVIC_DisableIRQ(TIMER_2_IRQn_2);
            break;
#endif
#if TIMER_3_EN
        case TIMER_3:
            NVIC_DisableIRQ(TIMER_3_IRQn_1);
            NVIC_DisableIRQ(TIMER_3_IRQn_2);
            break;
#endif

        case TIMER_UNDEFINED:
        default:
            return;
    }
}

#if TIMER_0_EN
void TIMER_0_ISR_1(void)
{
    if (config[0].cb != NULL) config[0].cb(0);

    if (sched_context_switch_request) {
        thread_yield();
    }

}

void TIMER_0_ISR_2(void)
{
    if (config[0].cb != NULL) config[0].cb(1);

    if (sched_context_switch_request) {
        thread_yield();
    }

}
#endif /* TIMER_0_EN */

#if TIMER_1_EN
void TIMER_1_ISR_1(void)
{
    if (config[1].cb != NULL) config[1].cb(0);

    if (sched_context_switch_request) {
        thread_yield();
    }

}

void TIMER_1_ISR_2(void)
{
    if (config[1].cb != NULL) config[1].cb(1);

    if (sched_context_switch_request) {
        thread_yield();
    }

}
#endif /* TIMER_1_EN */

#if TIMER_2_EN
void TIMER_2_ISR_1(void)
{
    if (config[2].cb != NULL) config[2].cb(0);

    if (sched_context_switch_request) {
        thread_yield();
    }

}

void TIMER_2_ISR_2(void)
{
    if (config[2].cb != NULL) config[2].cb(1);

    if (sched_context_switch_request) {
        thread_yield();
    }

}
#endif /* TIMER_2_EN */

#if TIMER_3_EN
void TIMER_3_ISR_1(void)
{
    if (config[3].cb != NULL) config[3].cb(0);

    if (sched_context_switch_request) {
        thread_yield();
    }

}

void TIMER_3_ISR_2(void)
{
    if (config[3].cb != NULL) config[3].cb(1);

    if (sched_context_switch_request) {
        thread_yield();
    }

}
#endif /* TIMER_3_EN */
