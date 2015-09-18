/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_cc430
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
static void (*isr_cb)(int chan);


int timer_init(tim_t dev, unsigned int us_per_tick, void (*callback)(int))
{
    /* using fixed TIMER_BASE for now */
    if (dev != 0) {
        return -1;
    }
    /* TODO: configure time-base depending on us_per_tick value */
    if (us_per_tick != 1) {
        return -1;
    }

    /* reset the timer A configuration */
    TIMER_BASE->CTL = CTL_CLR;
    /* save callback */
    isr_cb = callback;
    /* configure timer to use the SMCLK with prescaler of 8 */
    TIMER_BASE->CTL = (CTL_TASSEL_SMCLK | CTL_ID_DIV8);
    /* configure CC channels */
    for (int i = 0; i < TIMER_CHAN; i++) {
        TIMER_BASE->CCTL[i] = 0;
    }
    /* start the timer in continuous mode */
    TIMER_BASE->CTL |= CTL_MC_CONT;
    return 0;
}

int timer_set(tim_t dev, int channel, unsigned int timeout)
{
    uint16_t target = TIMER_BASE->R + (uint16_t)timeout;
    return timer_set_absolute(dev, channel, (unsigned int)target);
}

int timer_set_absolute(tim_t dev, int channel, unsigned int value)
{
    if (dev != 0 || channel > TIMER_CHAN) {
        return -1;
    }
    TIMER_BASE->CCR[channel] = value;
    TIMER_BASE->CCTL[channel] &= ~(CCTL_CCIFG);
    TIMER_BASE->CCTL[channel] |=  (CCTL_CCIE);
    return 0;
}

int timer_clear(tim_t dev, int channel)
{
    if (dev != 0 || channel > TIMER_CHAN) {
        return -1;
    }
    TIMER_BASE->CCTL[channel] &= ~(CCTL_CCIE);
    return 0;
}

unsigned int timer_read(tim_t dev)
{
    return (unsigned int)TIMER_BASE->R;
}

void timer_start(tim_t dev)
{
    TIMER_BASE->CTL |= CTL_MC_CONT;
}

void timer_stop(tim_t dev)
{
    TIMER_BASE->CTL &= ~(CTL_MC_MASK);
}

void timer_irq_enable(tim_t dev)
{

    /* TODO: not supported, yet
     *
     * Problem here: there is no means, of globally disabling timer interrupts.
     * We could just enable the interrupts for all CC channels, but this would
     * mean, that we might enable interrupts for channels, that are not active.
     * I guess we need to remember the interrupt state of all channels before
     * disabling and then restore this state when enabling again?! */
}

void timer_irq_disable(tim_t dev)
{
    /* TODO: not supported, yet */
}

void timer_reset(tim_t dev)
{
    TIMER_BASE->R = 0;
}

ISR(TIMER_ISR_CC0, isr_timer_a_cc0)
{
    __enter_isr();

    TIMER_BASE->CCTL[0] &= ~(CCTL_CCIE);
    isr_cb(0);

    __exit_isr();
}

ISR(TIMER_ISR_CCX, isr_timer_a_ccx_isr)
{
    __enter_isr();

    int chan = (int)(TIMER_BASE->IV >> 1);
    TIMER_BASE->CCTL[chan] &= ~(CCTL_CCIE);
    isr_cb(chan);

    __exit_isr();
}
