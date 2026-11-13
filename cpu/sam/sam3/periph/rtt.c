/*
 * SPDX-FileCopyrightText: 2017, 2020 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_sam3
 * @ingroup     drivers_periph_rtt
 *
 * @note        The hardware RTT unit does neither support overflow interrupts
 *              nor setting the counter value. For this, this RTT driver does
 *              not implement those functions.
 * @{
 *
 * @file
 * @brief       Low-level RTT driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "cpu.h"
#include "periph/rtt.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static struct {
    rtt_cb_t cb;
    void *arg;
} isr_ctx;

void rtt_init(void)
{
    /* enable RTT module */
    rtt_poweron();

    /* configure and apply the pre-scaler */
    RTT->RTT_MR = RTT_MR_RTPRES(CHIP_FREQ_XTAL_32K / RTT_FREQUENCY);
    RTT->RTT_MR |= RTT_MR_RTTRST;

    /* resetting the timer takes two slow clock cycles, so we wait for this to
     * complete */
    while (RTT->RTT_VR != 0) {}

    /* configure NVIC line */
    NVIC_EnableIRQ(RTT_IRQn);
}

uint32_t rtt_get_counter(void)
{
    return RTT->RTT_VR;
}

void rtt_set_alarm(uint32_t alarm, rtt_cb_t cb, void *arg)
{
    /* cancel any existing alarm */
    RTT->RTT_MR &= ~(RTT_MR_ALMIEN);

    /* set new alarm */
    isr_ctx.cb  = cb;
    isr_ctx.arg = arg;
    /* the alarm value is RTT_AR + 1, so we need to subtract 1 from the target
     * value here */
    RTT->RTT_AR = (alarm - 1);

    /* (re-)enable the alarm */
    RTT->RTT_MR |= RTT_MR_ALMIEN;
}

uint32_t rtt_get_alarm(void)
{
    if (RTT->RTT_MR & RTT_MR_ALMIEN) {
        /* the alarm value is RTT_AR + 1, see rtt_set_alarm() */
        return RTT->RTT_AR + 1;
    }
    return 0;
}

void rtt_clear_alarm(void)
{
    RTT->RTT_MR &= ~(RTT_MR_ALMIEN);
}

void rtt_poweron(void)
{
    PMC->PMC_PCER0 = (1 << ID_RTT);
}

void rtt_poweroff(void)
{
    PMC->PMC_PCDR0 = (1 << ID_RTT);
}

void isr_rtt(void)
{
    uint32_t state = RTT->RTT_SR;       /* this clears all pending flags */
    if (state & RTT_SR_ALMS) {
        RTT->RTT_MR &= ~(RTT_MR_ALMIEN);
        isr_ctx.cb(isr_ctx.arg);
    }

    cortexm_isr_end();
}
