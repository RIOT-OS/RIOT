/*
 * Copyright (C) 2018 INRIA
 *               2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_msp430fr59xx
 * @ingroup     drivers_periph_timer
 * @{
 *
 * @file
 * @brief       Low-level timer driver implementation
 *
 * This implementation does only support one fixed timer, as defined in the
 * boards periph_conf.h file.
 *
 * @todo        Generalize to handle more timers and make them configurable
 *              through the board's `periph_conf.h`
 *
 * @author      Loïc Saos <Loic.Saos@insa-lyon.fr>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "cpu.h"
#include "periph_cpu.h"
#include "periph_conf.h"
#include "periph/timer.h"

/**
 * @brief   Save reference to the timer callback
 */
static timer_cb_t isr_cb;

/**
 * @brief    Save argument for the ISR callback
 */
static void *isr_arg;

int timer_init(tim_t timer, unsigned long freq, timer_cb_t cb, void *arg)
{
    /* using fixed TIMER_BASE for now */
    if (timer != 0) {
        return -1;
    }

    /* save callback */
    isr_cb = cb;
    isr_arg = arg;

    msp_timer_t *const dev = TIMER_BASE;

    /* reset the timer A configuration */
    dev->CTL = TIMER_CTL_CLR;
    dev->CTL |= TIMER_CTL_TASSEL_SMCLK;

    if (CLOCK_CMCLK == freq) {
        dev->CTL |= TIMER_CTL_ID_DIV1;
    }
    else if (CLOCK_CMCLK / 2 == freq) {
        dev->CTL |= TIMER_CTL_ID_DIV2;
    }
    else if (CLOCK_CMCLK / 4 == freq) {
        dev->CTL |= TIMER_CTL_ID_DIV4;
    }
    else if (CLOCK_CMCLK / 8 == freq) {
        dev->CTL |= TIMER_CTL_ID_DIV8;
    }
    else {
        return -1;
    }

    /* configure CC channels */
    for (int i = 0; i < TIMER_CHAN; i++) {
        dev->CCTL[i] = 0;
    }

    /* start the timer in continuous mode */
    dev->CTL |= TIMER_CTL_MC_CONT;

    return 0;
}

int timer_set_absolute(tim_t timer, int channel, unsigned int value)
{
    if (timer != 0 || channel >= TIMER_CHAN) {
        return -1;
    }

    TIMER_BASE->CCR[channel] = value;
    TIMER_BASE->CCTL[channel] &= ~TIMER_CCTL_CCIFG;
    TIMER_BASE->CCTL[channel] |=  TIMER_CCTL_CCIE;

    return 0;
}

int timer_clear(tim_t timer, int channel)
{
    if (timer != 0 || channel >= TIMER_CHAN) {
        return -1;
    }

    TIMER_BASE->CCTL[channel] &= ~TIMER_CCTL_CCIE;

    return 0;
}

unsigned int timer_read(tim_t timer)
{
    (void)timer;

    return TIMER_BASE->R;
}

void timer_start(tim_t timer)
{
    (void)timer;

    TIMER_BASE->CTL |= TIMER_CTL_MC_CONT;
}

void timer_stop(tim_t timer)
{
    (void)timer;

    TIMER_BASE->CTL &= ~TIMER_CTL_MC_CONT;
}

ISR(TIMER_ISR_CC0, isr_timer_a_cc0)
{
    __enter_isr();

    TIMER_BASE->CCTL[0] &= ~TIMER_CCTL_CCIE;
    isr_cb(isr_arg, 0);

    __exit_isr();
}

ISR(TIMER_ISR_CCX, isr_timer_a_ccx)
{
    __enter_isr();

    const int chan = (int)(TIMER_BASE->IV >> 1);
    TIMER_BASE->CCTL[chan] &= ~TIMER_CCTL_CCIE;
    isr_cb(isr_arg, chan);

    __exit_isr();
}
