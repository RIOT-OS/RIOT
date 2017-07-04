/*
 * Copyright (C) 2015 Daniel Krebs
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_samd21
 * @ingroup     drivers_periph_rtt
 * @{
 *
 * @file
 * @brief       Low-level RTT driver implementation
 *
 * @author      Daniel Krebs <github@daniel-krebs.net>
 *
 * @}
 */

#include <time.h>
#include "cpu.h"
#include "periph/rtt.h"
#include "periph_conf.h"
#include "periph_clock_config.h"

/* if RTT_PRESCALER is not set, then set it to DIV1 */
#ifndef RTT_PRESCALER
#define RTT_PRESCALER       RTC_MODE0_CTRL_PRESCALER_DIV1
#endif

typedef struct {
    rtt_cb_t    overflow_cb;    /**< called from RTT interrupt on overflow */
    void*       overflow_arg;   /**< argument passed to overflow callback */

    rtt_cb_t    alarm_cb;       /**< called from RTT interrupt on alarm */
    void*       alarm_arg;      /**< argument passed to alarm callback */
} rtt_state_t;

static rtt_state_t rtt_callback;


/**
 * @brief Initialize RTT module
 *
 * The RTT is running at 32768 Hz by default, i.e. @ XOSC32K frequency without
 * divider. There are 2 cascaded dividers in the clock path:
 *
 *  - GCLK_GENDIV_DIV(n): between 1 and 31
 *  - RTC_MODE0_CTRL_PRESCALER_DIVn: between 1 and 1024,
 *    see defines in `component_rtc.h`
 *
 * However the division scheme of GCLK_GENDIV_DIV can be changed by setting
 * GCLK_GENCTRL_DIVSEL:
 *
 *  - GCLK_GENCTRL_DIVSEL = 0: Clock divided by GENDIV.DIV (default)
 *  - GCLK_GENCTRL_DIVSEL = 1: Clock divided by 2^( GENDIV.DIV + 1 )
 */
void rtt_init(void)
{
    RtcMode0 *rtcMode0 = &(RTT_DEV);

    /* Turn on power manager for RTC */
    PM->APBAMASK.reg |= PM_APBAMASK_RTC;

#if RTT_RUNSTDBY
    bool rtt_run_in_standby = true;
#else
    bool rtt_run_in_standby = false;
#endif
    setup_gen2_xosc32(rtt_run_in_standby);

    /* RTC uses GEN2_XOSC32 because OSC32K isn't accurate
     * enough (p1075/1138). Also keep running in standby. */
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_GEN_GCLK2 |
                        GCLK_CLKCTRL_CLKEN |
                        GCLK_CLKCTRL_ID(RTC_GCLK_ID);
    while (GCLK->STATUS.bit.SYNCBUSY) {}

    /* Disable RTC */
    rtt_poweroff();

    /* Reset RTC */
    rtcMode0->CTRL.bit.SWRST = 1;
    while (rtcMode0->STATUS.bit.SYNCBUSY || rtcMode0->CTRL.bit.SWRST) {}

    /* Configure as 32bit counter and no clear on match compare
     * RTT_PRESCALER can be configured in periph_conf.h */
    rtcMode0->CTRL.reg = RTC_MODE0_CTRL_MODE_COUNT32 |
                         RTT_PRESCALER;
    while (rtcMode0->STATUS.bit.SYNCBUSY) {}

    /* Setup interrupt */
    NVIC_EnableIRQ(RTT_IRQ);

    /* Enable RTC */
    rtt_poweron();
}

void rtt_set_overflow_cb(rtt_cb_t cb, void *arg)
{
    rtt_callback.overflow_cb = cb;
    rtt_callback.overflow_arg = arg;

    /* Enable Overflow Interrupt and clear flag */
    RtcMode0 *rtcMode0 = &(RTT_DEV);
    rtcMode0->INTFLAG.reg |= RTC_MODE0_INTFLAG_OVF;
    rtcMode0->INTENSET.bit.OVF = 1;
}

void rtt_clear_overflow_cb(void)
{
    /* Disable Overflow Interrupt */
    RtcMode0 *rtcMode0 = &(RTT_DEV);
    rtcMode0->INTENCLR.bit.OVF = 1;

    rtt_callback.overflow_cb = NULL;
    rtt_callback.overflow_arg = NULL;
}

uint32_t rtt_get_counter(void)
{
    RtcMode0 *rtcMode0 = &(RTT_DEV);
    while (rtcMode0->STATUS.bit.SYNCBUSY) {}
    return rtcMode0->COUNT.reg;
}

void rtt_set_counter(uint32_t counter)
{
    RtcMode0 *rtcMode0 = &(RTT_DEV);
    rtcMode0->COUNT.reg = counter;
    while (rtcMode0->STATUS.bit.SYNCBUSY) {}
}

void rtt_set_alarm(uint32_t alarm, rtt_cb_t cb, void *arg)
{
    rtt_callback.alarm_cb = cb;
    rtt_callback.alarm_arg = arg;

    RtcMode0 *rtcMode0 = &(RTT_DEV);
    rtcMode0->COMP[0].reg = alarm;
    while (rtcMode0->STATUS.bit.SYNCBUSY) {}

    /* Enable Compare Interrupt and clear flag */
    rtcMode0->INTFLAG.reg |= RTC_MODE0_INTFLAG_CMP0;
    rtcMode0->INTENSET.bit.CMP0 = 1;
}

void rtt_clear_alarm(void)
{
    /* Disable Compare Interrupt */
    RtcMode0 *rtcMode0 = &(RTT_DEV);
    rtcMode0->INTENCLR.bit.CMP0 = 1;

    rtt_callback.alarm_cb = NULL;
    rtt_callback.alarm_arg = NULL;
}

uint32_t rtt_get_alarm(void)
{
    RtcMode0 *rtcMode0 = &(RTT_DEV);
    return rtcMode0->COMP[0].reg;
}

void rtt_poweron(void)
{
    RtcMode0 *rtcMode0 = &(RTT_DEV);
    rtcMode0->CTRL.bit.ENABLE = 1;
    while (rtcMode0->STATUS.bit.SYNCBUSY) {}
}

void rtt_poweroff(void)
{
    RtcMode0 *rtcMode0 = &(RTT_DEV);
    rtcMode0->CTRL.bit.ENABLE = 0;
    while (rtcMode0->STATUS.bit.SYNCBUSY) {}
}

void RTT_ISR(void)
{
    RtcMode0 *rtcMode0 = &(RTT_DEV);
    uint8_t status = rtcMode0->INTFLAG.reg;

    if ((status & RTC_MODE0_INTFLAG_CMP0) &&
        (rtt_callback.alarm_cb != NULL) ) {
        rtt_callback.alarm_cb(rtt_callback.alarm_arg);
        rtcMode0->INTFLAG.reg |= RTC_MODE0_INTFLAG_CMP0;
    }

    if ((status & RTC_MODE0_INTFLAG_OVF) &&
        (rtt_callback.overflow_cb != NULL) ) {
        rtt_callback.overflow_cb(rtt_callback.overflow_arg);
        rtcMode0->INTFLAG.reg |= RTC_MODE0_INTFLAG_OVF;
    }

    cortexm_isr_end();
}
