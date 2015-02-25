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

#define TIMER_A_IRQ_MASK 0x000000ff
#define TIMER_B_IRQ_MASK 0x0000ff00

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

    /*
     * In 32-bit mode, the prescaler is not usable,
     * so the only frequency available is 16 MHz.
     */
    if (ticks_per_us != 16) {
        return -1;
    }

    gptimer_num = ((uintptr_t)gptimer - (uintptr_t)GPTIMER0) / 0x1000;

    /* Save the callback function: */
    config[dev].cb = callback;

    /* Enable the clock for this timer: */
    SYS_CTRL_RCGCGPT |= (1 << gptimer_num);

    /* Disable this timer before configuring it: */
    gptimer->cc2538_gptimer_ctl.CTL = 0;

    gptimer->CFG  = GPTMCFG_32_BIT_TIMER;
    gptimer->cc2538_gptimer_tamr.TAMR = GPTIMER_PERIODIC_MODE;
    gptimer->cc2538_gptimer_tamr.TAMRbits.TACDIR = 1; /**< Count up */
    gptimer->cc2538_gptimer_tamr.TAMRbits.TAMIE  = 1; /**< Enable the Timer A Match Interrupt */

    /* Enable interrupts for given timer: */
    timer_irq_enable(dev);

    /* Enable the timer: */
    gptimer->cc2538_gptimer_ctl.CTLbits.TAEN = 1;

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
    gptimer->ICR           = TIMER_A_IRQ_MASK; /**< Clear any pending interrupt status */
    gptimer->TAMATCHR      = value;
    gptimer->IMRbits.TAMIM = 1;                /**< Enable the Timer A Match Interrupt */

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

    gptimer->IMR = 0;

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
            return TIMER_0_DEV->TAV;
#endif
#if TIMER_1_EN
        case TIMER_1:
            return TIMER_1_DEV->TAV;
#endif
#if TIMER_2_EN
        case TIMER_2:
            return TIMER_2_DEV->TAV;
#endif
#if TIMER_3_EN
        case TIMER_3:
            return TIMER_3_DEV->TAV;
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
            TIMER_0_DEV->cc2538_gptimer_ctl.CTLbits.TAEN = 0;
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            TIMER_1_DEV->cc2538_gptimer_ctl.CTLbits.TAEN = 0;
            break;
#endif
#if TIMER_2_EN
        case TIMER_2:
            TIMER_2_DEV->cc2538_gptimer_ctl.CTLbits.TAEN = 0;
            break;
#endif
#if TIMER_3_EN
        case TIMER_3:
            TIMER_3_DEV->cc2538_gptimer_ctl.CTLbits.TAEN = 0;
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
            TIMER_0_DEV->cc2538_gptimer_ctl.CTLbits.TAEN = 1;
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            TIMER_1_DEV->cc2538_gptimer_ctl.CTLbits.TAEN = 1;
            break;
#endif
#if TIMER_2_EN
        case TIMER_2:
            TIMER_2_DEV->cc2538_gptimer_ctl.CTLbits.TAEN = 1;
            break;
#endif
#if TIMER_3_EN
        case TIMER_3:
            TIMER_3_DEV->cc2538_gptimer_ctl.CTLbits.TAEN = 1;
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
    TIMER_0_DEV->ICR = TIMER_A_IRQ_MASK;
    if (config[0].cb != NULL) config[0].cb(0);

    if (sched_context_switch_request) {
        thread_yield();
    }

}

void TIMER_0_ISR_2(void)
{
    TIMER_0_DEV->ICR = TIMER_B_IRQ_MASK;
    if (config[0].cb != NULL) config[0].cb(1);

    if (sched_context_switch_request) {
        thread_yield();
    }

}
#endif /* TIMER_0_EN */

#if TIMER_1_EN
void TIMER_1_ISR_1(void)
{
    TIMER_1_DEV->ICR = TIMER_A_IRQ_MASK;
    if (config[1].cb != NULL) config[1].cb(0);

    if (sched_context_switch_request) {
        thread_yield();
    }

}

void TIMER_1_ISR_2(void)
{
    TIMER_1_DEV->ICR = TIMER_B_IRQ_MASK;
    if (config[1].cb != NULL) config[1].cb(1);

    if (sched_context_switch_request) {
        thread_yield();
    }

}
#endif /* TIMER_1_EN */

#if TIMER_2_EN
void TIMER_2_ISR_1(void)
{
    TIMER_2_DEV->ICR = TIMER_A_IRQ_MASK;
    if (config[2].cb != NULL) config[2].cb(0);

    if (sched_context_switch_request) {
        thread_yield();
    }

}

void TIMER_2_ISR_2(void)
{
    TIMER_2_DEV->ICR = TIMER_B_IRQ_MASK;
    if (config[2].cb != NULL) config[2].cb(1);

    if (sched_context_switch_request) {
        thread_yield();
    }

}
#endif /* TIMER_2_EN */

#if TIMER_3_EN
void TIMER_3_ISR_1(void)
{
    TIMER_3_DEV->ICR = TIMER_A_IRQ_MASK;
    if (config[3].cb != NULL) config[3].cb(0);

    if (sched_context_switch_request) {
        thread_yield();
    }

}

void TIMER_3_ISR_2(void)
{
    TIMER_3_DEV->ICR = TIMER_B_IRQ_MASK;
    if (config[3].cb != NULL) config[3].cb(1);

    if (sched_context_switch_request) {
        thread_yield();
    }

}
#endif /* TIMER_3_EN */
