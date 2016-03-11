/*
 * Copyright (C) 2014-2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_sam3
 * @{
 *
 * @file
 * @brief       Low-level timer driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
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

/**
 * @brief   Memory to store the interrupt context
 */
static timer_isr_ctx_t isr_ctx[TIMER_NUMOF];

/**
 * @brief   Enable the clock for the selected timer channels
 */
static inline void clk_en(tim_t tim)
{
    uint8_t id = timer_config[tim].id_ch0;

    if (id < 32) {
        PMC->PMC_PCER0 = ((1 << id) | (1 << (id + 1)));
    } else {
        id -= 32;
        PMC->PMC_PCER1 = ((1 << id) | (1 << (id + 1)));
    }
}

/**
 * @brief   Get the timer ID from the timer's base address
 */
static inline Tc *dev(tim_t tim)
{
    return timer_config[tim].dev;
}

/**
 * @brief   Setup the given timer
 *
 * The SAM3X8E has 3 timers build of 3 independent channels. Each of these
 * channels has 3 capture compare outputs (A-C).
 *
 * RIOT uses the 2 of the channels in WAVE mode with the following clock
 * chaining:
 *
 * ----------                ----------
 * |        |                |        |-> IRQ-compareA
 * | TCx[1] | ---- TIOA1 --->| TCx[0] |-> IRQ-compareB
 * |        |                |        |-> IRQ-compareC
 * ----------                ----------
 *     ^
 * TIMER_CLOCK1
 *
 * For each timer, channel 1 is used to implement a prescaler. Channel 1 is
 * driven by the MCK / 2 (42MHz) (TIMER_CLOCK1).
 */
int timer_init(tim_t tim, unsigned long freq, timer_cb_t cb, void *arg)
{
    /* check if device is valid */
    if (tim >= TIMER_NUMOF) {
        return -1;
    }

    /* enable the device clock */
    clk_en(tim);

    /* save callback */
    isr_ctx[tim].cb = cb;
    isr_ctx[tim].arg = arg;

    /* configure the timer block by connecting TIOA1 to XC0 */
    dev(tim)->TC_BMR = TC_BMR_TC0XC0S_TIOA1;
    /* configure and enable channel 0 to use XC0 as input */
    dev(tim)->TC_CHANNEL[0].TC_CMR = (TC_CMR_TCCLKS_XC0 |
                                 TC_CMR_WAVE | TC_CMR_EEVT_XC0);
    dev(tim)->TC_CHANNEL[0].TC_CCR = (TC_CCR_CLKEN | TC_CCR_SWTRG);

    /* configure channel 1:
     * - select wave mode
     * - set input clock to TIMER_CLOCK1 (MCK/2)
     * - reload on TC_CV == TC_RC
     * - let TIOA2 signal be toggled when TC_CV == TC_RC
     */
    dev(tim)->TC_CHANNEL[1].TC_CMR = (TC_CMR_TCCLKS_TIMER_CLOCK1 | TC_CMR_WAVE |
                                      TC_CMR_WAVSEL_UP_RC | TC_CMR_ACPC_TOGGLE);

    /* configure the frequency of channel 1 to freq * 4
     *
     * note: as channel 0 is only incremented on rising edges of TIOA1 line and
     * channel 1 toggles this line on each timer tick, the actual frequency
     * driving channel 0 is f_ch2 / 2 --> f_ch0/1 = (MCK / 2) / 2 / freq.
     */
    dev(tim)->TC_CHANNEL[1].TC_R[2] = (CLOCK_CORECLOCK / 4) / freq;

    /* start channel 1 */
    dev(tim)->TC_CHANNEL[1].TC_CCR = (TC_CCR_CLKEN | TC_CCR_SWTRG);

    /* enable global interrupts for given timer */
    timer_irq_enable(tim);

    return 0;
}

int timer_set(tim_t tim, int channel, unsigned int timeout)
{
    return timer_set_absolute(tim, channel, timer_read(tim) + timeout);
}

int timer_set_absolute(tim_t tim, int channel, unsigned int value)
{
    if (channel >=TIMER_CHANNELS) {
        return -1;
    }

    dev(tim)->TC_CHANNEL[0].TC_R[channel] = value;
    dev(tim)->TC_CHANNEL[0].TC_IER = (TC_IER_CPAS << channel);

    return 0;
}

int timer_clear(tim_t tim, int channel)
{
    if (channel >= TIMER_CHANNELS) {
        return -1;
    }

    dev(tim)->TC_CHANNEL[0].TC_IDR = (TC_IDR_CPAS << channel);

    return 1;
}

unsigned int timer_read(tim_t tim)
{
    return dev(tim)->TC_CHANNEL[0].TC_CV;
}

void timer_start(tim_t tim)
{
    dev(tim)->TC_CHANNEL[1].TC_CCR = (TC_CCR_CLKEN | TC_CCR_SWTRG);
}

void timer_stop(tim_t tim)
{
    dev(tim)->TC_CHANNEL[1].TC_CCR = TC_CCR_CLKDIS;
}

void timer_irq_enable(tim_t tim)
{
    NVIC_EnableIRQ(timer_config[tim].id_ch0);
}

void timer_irq_disable(tim_t tim)
{
    NVIC_DisableIRQ(timer_config[tim].id_ch0);
}

static inline void isr_handler(tim_t tim)
{
    uint32_t status = dev(tim)->TC_CHANNEL[0].TC_SR;

    for (int i = 0; i < TIMER_CHANNELS; i++) {
        if (status & (TC_SR_CPAS << i)) {
            dev(tim)->TC_CHANNEL[0].TC_IDR = (TC_IDR_CPAS << i);
            isr_ctx[tim].cb(isr_ctx[tim].arg, i);
        }
    }

    if (sched_context_switch_request) {
        thread_yield();
    }
}

#ifdef TIMER_0_ISR
void TIMER_0_ISR(void)
{
    isr_handler(0);
}
#endif

#ifdef TIMER_1_ISR
void TIMER_1_ISR(void)
{
    isr_handler(1);
}
#endif

#ifdef TIMER_2_ISR
void TIMER_2_ISR(void)
{
    isr_handler(2);
}
#endif
