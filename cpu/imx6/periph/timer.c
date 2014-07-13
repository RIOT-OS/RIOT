/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     driver_periph
 * @{
 *
 * @file        timer.c
 * @brief       Low-level timer driver implementation for the SAM3X8E CPU
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdlib.h>
#include <stdio.h>

#include "board.h"
#include "cpu.h"

#include "periph/timer.h"
#include "periph_conf.h"

#include "timer/timer.h"
#include "timer/gpt.h"
#include "timer/epit.h"

/* See imx6sdl reference manual: GPT (p. 1591) */

typedef struct {
    void (*cb)(int);
} timer_conf_t;

/**
 * @brief Timer state memory
 */
timer_conf_t config[TIMER_NUMOF];
/**
 * @brieft active GPT channes. (1 << 0) is the first channel, (1 << 1) the second and (1 << 2) the third
 */
uint8_t active_gpt_channels = 0;


#if TIMER_0_EN
__attribute__((naked)) void isr_tc0(void) ;
#endif
#if TIMER_1_EN
__attribute__((naked)) void isr_tc1(void) ;
#endif
#if TIMER_2_EN
__attribute__((naked)) void isr_tc2(void) ;
#endif
/**
 * @brief Setup the given timer
 *
 * The RIoTboard has 3 timers. One GPT with three channels. Two EPITs with one channel each.
 * The second EPIT-instance can be - and currently is being - used for a delay-function.
 *
 *           ----------
 *           |        |-> IRQ-compareA
 *   ------->| GPT    |-> IRQ-compareB
 *   |       |        |-> IRQ-compareC
 *   |       ----------
 *   |
 *   |       ----------
 *   |       |        |
 *   ------->| EPIT1  |-> IRQ-compareA
 *           |        |
 *           ----------
 *
 * Maybe I should ask Hauke to help with the ASCII-art
 * All timers use the HWTIMER_CLKSRC with HWTIMER_PRESCALE.
 */
int timer_init(tim_t dev, unsigned int ticks_per_us, void (*callback)(int))
{
    switch (dev) {
#if TIMER_0_EN

        case TIMER_0:
            config[TIMER_0].cb = callback;
#if TIMER_0_ISEPIT
            epit_init(TIMER_0_EPITDEV, HWTIMER_CLKSRC, HWTIMER_PRESCALE, FREE_RUNNING, 0, WAIT_MODE_EN | STOP_MODE_EN);
            epit_setup_interrupt(TIMER_0_EPITDEV, isr_tc0, 1);
            epit_counter_enable(TIMER_0_EPITDEV, 0, IRQ_MODE);
#else
            gpt_init(HWTIMER_CLKSRC, HWTIMER_PRESCALE, FREE_RUN_MODE, WAIT_MODE_EN | STOP_MODE_EN);
            gpt_setup_interrupt(isr_tc0, 1);
            gpt_counter_enable(active_gpt_channels);
#endif
            break;
#endif
#if TIMER_1_EN

        case TIMER_1:
            config[TIMER_1].cb = callback;
#if TIMER_1_ISEPIT
            epit_init(TIMER_1_EPITDEV, HWTIMER_CLKSRC, HWTIMER_PRESCALE, FREE_RUNNING, 0, WAIT_MODE_EN | STOP_MODE_EN);
            epit_setup_interrupt(TIMER_1_EPITDEV, isr_tc1, 1);
            epit_counter_enable(TIMER_1_EPITDEV, 0, IRQ_MODE);
#else
            gpt_init(HWTIMER_CLKSRC, HWTIMER_PRESCALE, FREE_RUN_MODE, WAIT_MODE_EN | STOP_MODE_EN);
            gpt_setup_interrupt(isr_tc1, 1);
            gpt_counter_enable(active_gpt_channels);
#endif
            break;
#endif
#if TIMER_2_EN

        case TIMER_2:
            config[TIMER_2].cb = callback;
#if TIMER_2_ISEPIT
            epit_init(TIMER_2_EPITDEV, HWTIMER_CLKSRC, HWTIMER_PRESCALE, FREE_RUNNING, 0, WAIT_MODE_EN | STOP_MODE_EN);
            epit_setup_interrupt(TIMER_2_EPITDEV, isr_tc2, 1);
            epit_counter_enable(TIMER_2_EPITDEV, 0, IRQ_MODE);
#else
            gpt_init(HWTIMER_CLKSRC, HWTIMER_PRESCALE, FREE_RUN_MODE, WAIT_MODE_EN | STOP_MODE_EN);
            gpt_setup_interrupt(isr_tc2, 1);
            gpt_counter_enable(active_gpt_channels);
#endif
            break;
#endif

        case TIMER_UNDEFINED:
        default:
            return -1;
    }

    return 0;
}

int timer_set(tim_t dev, int channel, unsigned int timeout)
{
    return timer_set_absolute(dev, channel, timer_read(dev) + timeout);
}

int timer_set_absolute(tim_t dev, int channel, unsigned int value)
{
    bool is_gpt = false;

    /* get timer base register address */
    switch (dev) {
#if TIMER_0_EN

        case TIMER_0:
#if TIMER_0_ISEPIT
            epit_set_compare_event(TIMER_0_EPITDEV, TIMER_0_MAX_VALUE - value);
            epit_counter_enable(TIMER_0_EPITDEV, 0, IRQ_MODE);
#else
            is_gpt = true;
#endif
            break;
#endif
#if TIMER_1_EN

        case TIMER_1:
#if TIMER_1_ISEPIT
            epit_set_compare_event(TIMER_1_EPITDEV, TIMER_1_MAX_VALUE - value);
            epit_counter_enable(TIMER_1_EPITDEV, 0, IRQ_MODE);
#else
            is_gpt = true;
#endif
            break;
#endif
#if TIMER_2_EN

        case TIMER_2:
#if TIMER_2_ISEPIT
            epit_set_compare_event(TIMER_2_EPITDEV, TIMER_2_MAX_VALUE - value);
            epit_counter_enable(TIMER_2_EPITDEV, 0, IRQ_MODE);
#else
            is_gpt = true;
#endif
            break;
#endif

        case TIMER_UNDEFINED:
        default:
            return -1;
    }

    if (is_gpt) {
        gpt_set_compare_event((1 << channel), OUTPUT_CMP_SET, value);

        /* Set the correspoing flags for raising interrupts */
        active_gpt_channels |= (1 << channel);
        gpt_counter_enable(active_gpt_channels);
    }

    return 1;
}

int timer_clear(tim_t dev, int channel)
{
    switch (dev) {
#if TIMER_0_EN

        case TIMER_0:
#if TIMER_0_ISEPIT
            epit_counter_disable(TIMER_0_EPITDEV);
#else
            active_gpt_channels &= (~(1 << channel));

            if (active_gpt_channels == 0) {
                gpt_counter_disable();
            }
            else {
                gpt_counter_enable(active_gpt_channels);
            }

#endif
            break;
#endif
#if TIMER_1_EN

        case TIMER_1:
#if TIMER_1_ISEPIT
            epit_counter_disable(TIMER_1_EPITDEV);
#else
            active_gpt_channels &= (~(1 << channel));

            if (active_gpt_channels == 0) {
                gpt_counter_disable();
            }
            else {
                gpt_counter_enable(active_gpt_channels);
            }

#endif
            break;
#endif
#if TIMER_2_EN

        case TIMER_2:
#if TIMER_2_ISEPIT
            epit_counter_disable(TIMER_2_EPITDEV);
#else
            active_gpt_channels &= (~(1 << channel));

            if (active_gpt_channels == 0) {
                gpt_counter_disable();
            }
            else {
                gpt_counter_enable(active_gpt_channels);
            }

#endif
            break;
#endif

        case TIMER_UNDEFINED:
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
            /* EPIT is counting downwards; GPT upwards */
#if TIMER_0_EN
        case TIMER_0:
#if TIMER_0_ISEPIT
            return TIMER_0_MAX_VALUE - epit_get_counter_value(TIMER_0_EPITDEV);
#else
            return HW_GPT_CNT_RD();
#endif
            break;
#endif
#if TIMER_1_EN

        case TIMER_1:
#if TIMER_1_ISEPIT
            return TIMER_1_MAX_VALUE - epit_get_counter_value(TIMER_1_EPITDEV);
#else
            return HW_GPT_CNT_RD();
#endif
            break;
#endif
#if TIMER_2_EN

        case TIMER_2:
#if TIMER_2_ISEPIT
            return TIMER_2_MAX_VALUE - epit_get_counter_value(TIMER_2_EPITDEV);
#else
            return HW_GPT_CNT_RD();
#endif
            break;
#endif

        case TIMER_UNDEFINED:
        default:
            return 0;
    }
}

/*
 * For stopping the counting of channel 0 is disabled.
 */
void timer_stop(tim_t dev)
{
    switch (dev) {
#if TIMER_0_EN

        case TIMER_0:
#if TIMER_0_ISEPIT
            epit_counter_disable(TIMER_0_EPITDEV);
#else
            gpt_counter_disable();
#endif
            break;
#endif
#if TIMER_1_EN

        case TIMER_1:
#if TIMER_1_ISEPIT
            epit_counter_disable(TIMER_1_EPITDEV);
#else
            gpt_counter_disable();
#endif
            break;
#endif
#if TIMER_2_EN

        case TIMER_2:
#if TIMER_2_ISEPIT
            epit_counter_disable(TIMER_2_EPITDEV);
#else
            gpt_counter_disable();
#endif
            break;
#endif

        case TIMER_UNDEFINED:
        default:
            break;
    }
}

void timer_start(tim_t dev)
{
    switch (dev) {
#if TIMER_0_EN

        case TIMER_0:
#if TIMER_0_ISEPIT
            epit_counter_enable(TIMER_0_EPITDEV, 0, IRQ_MODE);
#else
            gpt_counter_enable(active_gpt_channels);
#endif
            break;
#endif
#if TIMER_1_EN

        case TIMER_1:
#if TIMER_1_ISEPIT
            epit_counter_enable(TIMER_1_EPITDEV, 0, IRQ_MODE);
#else
            gpt_counter_enable(active_gpt_channels);
#endif
            break;
#endif
#if TIMER_2_EN

        case TIMER_2:
#if TIMER_2_ISEPIT
            epit_counter_enable(TIMER_2_EPITDEV, 0, IRQ_MODE);
#else
            gpt_counter_enable(active_gpt_channels);
#endif
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

        case TIMER_0:
#if TIMER_0_ISEPIT
            epit_setup_interrupt(TIMER_0_EPITDEV, isr_tc0, 1);
#else
            gpt_setup_interrupt(isr_tc0, 1);
#endif
            break;
#endif
#if TIMER_1_EN

        case TIMER_1:
#if TIMER_1_ISEPIT
            epit_setup_interrupt(TIMER_1_EPITDEV, isr_tc1, 1);
#else
            gpt_setup_interrupt(isr_tc1, 1);
#endif
            break;
#endif
#if TIMER_2_EN

        case TIMER_2:
#if TIMER_2_ISEPIT
            epit_setup_interrupt(TIMER_2_EPITDEV, isr_tc2, 1);
#else
            gpt_setup_interrupt(isr_tc2, 1);
#endif
            break;
#endif

        case TIMER_UNDEFINED:
            break;
    }
}

void timer_irq_disable(tim_t dev)
{
    switch (dev) {
#if TIMER_0_EN

        case TIMER_0:
#if TIMER_0_ISEPIT
            epit_setup_interrupt(TIMER_0_EPITDEV, isr_tc0, 0);
#else
            gpt_setup_interrupt(isr_tc0, 0);
#endif
            break;
#endif
#if TIMER_1_EN

        case TIMER_1:
#if TIMER_1_ISEPIT
            epit_setup_interrupt(TIMER_1_EPITDEV, isr_tc1, 0);
#else
            gpt_setup_interrupt(isr_tc1, 0);
#endif
            break;
#endif
#if TIMER_2_EN

        case TIMER_2:
#if TIMER_2_ISEPIT
            epit_setup_interrupt(TIMER_2_EPITDEV, isr_tc2, 0);
#else
            gpt_setup_interrupt(isr_tc2, 0);
#endif
            break;
#endif

        case TIMER_UNDEFINED:
            break;
    }
}

void timer_reset(tim_t dev)
{
    switch (dev) {
#if TIMER_0_EN

        case TIMER_0:
#if TIMER_0_ISEPIT
            epit_reload_counter(TIMER_0_EPITDEV, TIMER_0_MAX_VALUE);
#else
            /* TODO GPT_CNT cant be written to */
#endif
            break;
#endif
#if TIMER_1_EN

        case TIMER_1:
#if TIMER_1_ISEPIT
            epit_reload_counter(TIMER_1_EPITDEV, TIMER_1_MAX_VALUE);
#else
            /* TODO GPT_CNT cant be written to */
#endif
            break;
#endif
#if TIMER_2_EN

        case TIMER_2:
#if TIMER_2_ISEPIT
            epit_reload_counter(TIMER_2_EPITDEV, TIMER_2_MAX_VALUE);
#else
            /* TODO GPT_CNT cant be written to */
#endif
            break;
#endif

        default:
        case TIMER_UNDEFINED:
            break;
    }
}


#if TIMER_0_EN
__attribute__((naked))
void isr_tc0(void)
{
    //disableIRQ();
#if TIMER_0_ISEPIT
    if (epit_get_compare_event(TIMER_0_EPITDEV)) {
        config[TIMER_0].cb(0);
    }

#else

    if (gpt_get_compare_event(kGPTOutputCompare1)) {
        config[TIMER_0].cb(0);
    }

    if (gpt_get_compare_event(kGPTOutputCompare2)) {
        config[TIMER_0].cb(1);
    }

    if (gpt_get_compare_event(kGPTOutputCompare3)) {
        config[TIMER_0].cb(2);
    }

#endif
    //enableIRQ();
}
#endif /* TIMER_0_EN */
#if TIMER_1_EN
__attribute__((naked))
void isr_tc1(void)
{
    //disableIRQ();
#if TIMER_1_ISEPIT
    if (epit_get_compare_event(TIMER_1_EPITDEV)) {
        config[TIMER_1].cb(0);
    }

#else

    if (gpt_get_compare_event(kGPTOutputCompare1)) {
        config[TIMER_1].cb(0);
    }

    if (gpt_get_compare_event(kGPTOutputCompare2)) {
        config[TIMER_1].cb(1);
    }

    if (gpt_get_compare_event(kGPTOutputCompare3)) {
        config[TIMER_1].cb(2);
    }

#endif
    //enableIRQ();
}
#endif /* TIMER_1_EN */
#if TIMER_2_EN
__attribute__((naked))
void isr_tc2(void)
{
    //disableIRQ();
#if TIMER_2_ISEPIT
    if (epit_get_compare_event(TIMER_2_EPITDEV)) {
        config[TIMER_2].cb(0);
    }

#else

    if (gpt_get_compare_event(kGPTOutputCompare1)) {
        config[TIMER_2].cb(0);
    }

    if (gpt_get_compare_event(kGPTOutputCompare2)) {
        config[TIMER_2].cb(1);
    }

    if (gpt_get_compare_event(kGPTOutputCompare3)) {
        config[TIMER_2].cb(2);
    }

#endif
    //enableIRQ();
}
#endif /* TIMER_2_EN */
