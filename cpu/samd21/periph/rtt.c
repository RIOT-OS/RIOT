/*
 * Copyright (C) 2015 Daniel Krebs
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_samd21
 * @{
 * @file
 * @brief       Low-level RTT driver implementation
 * @author      Daniel Krebs <github@daniel-krebs.net>
 * @}
 */

#include <time.h>
#include "cpu.h"
#include "periph/rtt.h"
#include "periph_conf.h"
#include "sched.h"
#include "thread.h"

/* guard file in case no RTT device was specified */
#if RTT_NUMOF


typedef struct {
    rtt_cb_t    overflow_cb;    /**< called from RTT interrupt on overflow */
    void*       overflow_arg;   /**< argument passed to overflow callback */

    rtt_cb_t    alarm_cb;       /**< called from RTT interrupt on alarm */
    void*       alarm_arg;      /**< argument passen to alarm callback */
} rtt_state_t;

static rtt_state_t rtt_callback;


/**
 * @brief Initialize RTT module
 *
 * The RTT is running at 32768 Hz by default, i.e. @ XOSC32K frequency without
 * divider. There are 2 cascaded dividers in the clock path:
 *
 *  - GCLK_GENDIV_DIV(n): between 1 and 31
 *  - RTC_MODE0_CTRL_PRESCALER_DIVn: between 1 and 1024, see defines in `component_rtc.h`
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

    /* RTC uses External 32,768KHz Oscillator because OSC32K isn't accurate
     * enough (p1075/1138). Also keep running in standby. */
    SYSCTRL->XOSC32K.reg =  SYSCTRL_XOSC32K_ONDEMAND |
                            SYSCTRL_XOSC32K_EN32K |
                            SYSCTRL_XOSC32K_XTALEN |
                            SYSCTRL_XOSC32K_STARTUP(6) |
#if RTT_RUNSTDBY
                            SYSCTRL_XOSC32K_RUNSTDBY |
#endif
                            SYSCTRL_XOSC32K_ENABLE;

    /* Setup clock GCLK2 with divider 1 */
    GCLK->GENDIV.reg = GCLK_GENDIV_ID(2) | GCLK_GENDIV_DIV(1);
    while (GCLK->STATUS.bit.SYNCBUSY);

    /* Enable GCLK2 with XOSC32K as source. Use divider without modification
     * and keep running in standby. */
    GCLK->GENCTRL.reg = GCLK_GENCTRL_ID(2) |
                        GCLK_GENCTRL_GENEN |
#if RTT_RUNSTDBY
                        GCLK_GENCTRL_RUNSTDBY |
#endif
                        GCLK_GENCTRL_SRC_XOSC32K;
    while (GCLK->STATUS.bit.SYNCBUSY);

    /* Connect GCLK2 to RTC */
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_GEN_GCLK2 |
                        GCLK_CLKCTRL_CLKEN |
                        GCLK_CLKCTRL_ID(RTC_GCLK_ID);
    while (GCLK->STATUS.bit.SYNCBUSY);

    /* Disable RTC */
    rtt_poweroff();

    /* Reset RTC */
    rtcMode0->CTRL.bit.SWRST = 1;
    while (rtcMode0->STATUS.bit.SYNCBUSY || rtcMode0->CTRL.bit.SWRST);

    /* Configure as 32bit counter with no prescaler and no clear on match compare */
    rtcMode0->CTRL.reg = RTC_MODE0_CTRL_MODE_COUNT32 | RTC_MODE0_CTRL_PRESCALER_DIV1;
    while (rtcMode0->STATUS.bit.SYNCBUSY);

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
    rtcMode0->INTFLAG.bit.OVF = 1;
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
    while (rtcMode0->STATUS.bit.SYNCBUSY);
    return rtcMode0->COUNT.reg;
}

void rtt_set_counter(uint32_t counter)
{
    RtcMode0 *rtcMode0 = &(RTT_DEV);
    rtcMode0->COUNT.reg = counter;
    while (rtcMode0->STATUS.bit.SYNCBUSY);
}

void rtt_set_alarm(uint32_t alarm, rtt_cb_t cb, void *arg)
{
    rtt_callback.alarm_cb = cb;
    rtt_callback.alarm_arg = arg;

    RtcMode0 *rtcMode0 = &(RTT_DEV);
    rtcMode0->COMP[0].reg = alarm;
    while (rtcMode0->STATUS.bit.SYNCBUSY);

    /* Enable Compare Interrupt and clear flag */
    rtcMode0->INTFLAG.bit.CMP0 = 1;
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
    while (rtcMode0->STATUS.bit.SYNCBUSY);
}

void rtt_poweroff(void)
{
    RtcMode0 *rtcMode0 = &(RTT_DEV);
    rtcMode0->CTRL.bit.ENABLE = 0;
    while (rtcMode0->STATUS.bit.SYNCBUSY);
}

void RTT_ISR(void)
{
    RtcMode0 *rtcMode0 = &(RTT_DEV);
    uint8_t status = rtcMode0->INTFLAG.reg;

    if ( (status & RTC_MODE0_INTFLAG_CMP0) && (rtt_callback.alarm_cb != NULL) ) {
        rtt_callback.alarm_cb(rtt_callback.alarm_arg);
        rtcMode0->INTFLAG.bit.CMP0 = 1;
    }

    if ( (status & RTC_MODE0_INTFLAG_OVF) && (rtt_callback.overflow_cb != NULL) ) {
        rtt_callback.overflow_cb(rtt_callback.overflow_arg);
        rtcMode0->INTFLAG.bit.OVF = 1;
    }

    if (sched_context_switch_request) {
        thread_yield();
    }
}


#endif /* RTT_NUMOF */
