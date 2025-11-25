/*
 * Copyright (C) 2020 iosabi
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_qn908x
 * @ingroup     drivers_periph_rtc
 *
 * @{
 *
 * @file
 * @brief       Low-level Real-Time Clock (RTC) driver implementation
 *
 * @author      iosabi <iosabi@protonmail.com>
 *
 * @}
 */

#include <errno.h>
#include <stdlib.h>

#include "cpu.h"
#include "board.h"
#include "periph_conf.h"
#include "periph/rtc.h"

#include "vendor/drivers/fsl_clock.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/* Callback context. */
typedef struct {
    rtc_alarm_cb_t cb;
    void *arg;
    uint32_t alarm;
} rtc_ctx_t;

static rtc_ctx_t rtc_ctx = { NULL, NULL, 0 };

void rtc_init(void)
{
    DEBUG("rtc_init(), rtc=%" PRIu32 "\n", RTC->SEC);
    /* The RTC is really meant to be using the RCO32K at 32KHz. However, if the
     * 32K clock source is set to the external XTAL at 32.768 KHz we can use a
     * calibration parameter to correct for this:
     *  ppm = (32000 / 32768 - 1) * (1 << 20) = -24576
     */
#if CONFIG_CPU_CLK_32K_RCO
    /* 32000 Hz, no need to use the calibration. */
    RTC->CTRL &= ~RTC_CTRL_CAL_EN_MASK;
#elif CONFIG_CPU_CLK_32K_XTAL
    /* 32768 Hz, use -24576 ppm correction. */
    /* Positive ppm values would have the RTC_CAL_DIR_MASK set, but this is
     * negative. */
    RTC->CAL = RTC_CAL_PPM(24576);
    RTC->CTRL |= RTC_CTRL_CAL_EN_MASK;
#else
#error "One of the CONFIG_CPU_CLK_32K_* must be set."
#endif
    /* RTC clock (BIV) starts enabled after reset anyway. */
    CLOCK_EnableClock(kCLOCK_Biv);

    RTC->CTRL &= ~RTC_CTRL_SEC_INT_EN_MASK;

    /* We only use the RTC_SEC_IRQ which triggers every second if enabled by the
     * alarm. */
    NVIC_EnableIRQ(RTC_SEC_IRQn);
}

int rtc_set_time(struct tm *time)
{
    uint32_t ts = rtc_mktime(time);

    DEBUG("rtc_set_time(%" PRIu32 ")\n", ts);
    /* Writing 1 to CFG in CTRL register sets the CNT 0 timer to 0, resetting
     * the fractional part of the second, meaning that "SEC" is a round number
     * of second when this instruction executes. */
    RTC->CTRL |= RTC_CTRL_CFG_MASK;
    RTC->SEC = ts;
    while (RTC->STATUS &
           (RTC_STATUS_SEC_SYNC_MASK | RTC_STATUS_CTRL_SYNC_MASK)) {}
    return 0;
}

int rtc_get_time(struct tm *time)
{
    uint32_t ts = RTC->SEC;

    DEBUG("rtc_get_time() -> %" PRIu32 "\n", ts);
    rtc_localtime(ts, time);
    return 0;
}

int rtc_set_alarm(struct tm *time, rtc_alarm_cb_t cb, void *arg)
{
    uint32_t ts = rtc_mktime(time);

    DEBUG("rtc_set_alarm(%" PRIu32 ", %p, %p)\n", ts, cb, arg);

    if (ts <= RTC->SEC) {
        /* The requested time is in the past at the time of executing this
         * instruction, so we return invalid time. */
        return -EINVAL;
    }

    /* If the requested time arrives (SEC_INT should have fired) before we get
     * to set the RTC_CTRL_SEC_INT_EN_MASK mask a few instruction below, the
     * alarm will be 1 second late. */
    rtc_ctx.cb = cb;
    rtc_ctx.arg = arg;
    rtc_ctx.alarm = ts;

    RTC->CTRL |= RTC_CTRL_SEC_INT_EN_MASK;
    /* Wait until the CTRL_SEC is synced. */
    while (RTC->STATUS & RTC_STATUS_CTRL_SYNC_MASK) {}

    return 0;
}

int rtc_get_alarm(struct tm *time)
{
    DEBUG("rtc_clear_alarm() -> %" PRIu32 "\n", rtc_ctx.alarm);
    rtc_localtime(rtc_ctx.alarm, time);
    return 0;
}

void rtc_clear_alarm(void)
{
    DEBUG("rtc_clear_alarm()\n");
    /* Disable the alarm flag before clearing out the callback. */
    RTC->CTRL &= ~RTC_CTRL_SEC_INT_EN_MASK;
    rtc_ctx.cb = NULL;
    rtc_ctx.alarm = 0;
}

void rtc_poweron(void)
{
    /* We don't power on/off the RTC since it is shared with the timer module.
     * Besides the CLK_32K clock source for every peripheral that might use it,
     * there isn't much to turn off here. */
}

void rtc_poweroff(void)
{
    /* TODO: Coordinate with the RTT module to turn off the RTC clock when
     * neither one is in use. */
}

/**
 * @brief Interrupt service declared in vectors_qn908x.h
 *
 * We can only generate an interrupt every second, so we check the alarm value
 * every time. For a hardware based comparison use the timer instead.
 */
void isr_rtc_sec(void)
{
    if (RTC_STATUS_SEC_INT_MASK & RTC->STATUS) {
        DEBUG("isr_rtc_sec at %" PRIu32 "\n", RTC->SEC);
        /* Write 1 to clear the STATUS flag. */
        RTC->STATUS = RTC_STATUS_SEC_INT_MASK;
        if (rtc_ctx.cb != NULL && rtc_ctx.alarm <= RTC->SEC) {
            rtc_alarm_cb_t cb = rtc_ctx.cb;
            rtc_ctx.cb = NULL;
            /* Disable the interrupt. The cb may call rtc_set_alarm() again,
             * but otherwise we don't need the interrupt anymore. */
            RTC->CTRL &= ~RTC_CTRL_SEC_INT_EN_MASK;

            cb(rtc_ctx.arg);
        }
    }
    cortexm_isr_end();
}
