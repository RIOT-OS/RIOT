/*
 * SPDX-FileCopyrightText: 2015 Eistec AB
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_kinetis
 * @ingroup     drivers_periph_rtc
 *
 * @{
 *
 * @file
 * @brief       RTC interface wrapper for use with RTT modules.
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

#include "bit.h"
#include "cpu.h"
#include "periph/rtc.h"
#include "periph_conf.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static rtc_alarm_cb_t rtc_callback = NULL;
static rtc_alarm_cb_t rtc_arg;

void rtc_init(void)
{
    rtc_callback = NULL;

   /* Enable module clock gate */
    RTC_CLKEN();

    /* At this point, the CPU core may be clocked by a clock derived from the
     * RTC oscillator, avoid touching the oscillator enable bit (OSCE) in RTC_CR */

    /* Enable user mode access */
    bit_set32(&RTC->CR, RTC_CR_SUP_SHIFT);

    /* Disable all RTC interrupts. */
    RTC->IER = 0;

    /* The RTC module is only reset on VBAT power on reset, we try to preserve
     * the seconds counter between reboots */
    if (RTC->SR & RTC_SR_TIF_MASK) {
        /* Time Invalid Flag is set, clear TIF by writing TSR */

        /* Stop counter to make TSR writable */
        bit_clear32(&RTC->SR, RTC_SR_TCE_SHIFT);

        RTC->TSR = 0;
    }

    /* Clear the alarm flag TAF by writing a new alarm target to TAR */
    RTC->TAR = 0xffffffff;

    /* Enable RTC interrupts */
    NVIC_EnableIRQ(RTC_IRQn);

}

int rtc_set_time(struct tm *time)
{
    uint32_t t = rtc_mktime(time);

    /* Disable time counter before writing to the timestamp register */
    bit_clear32(&RTC->SR, RTC_SR_TCE_SHIFT);
    RTC->TPR = 0;
    /* write TSR after TPR, as clearing TPR bit 14 will increment TSR */
    RTC->TSR = t;
    /* Enable when done */
    bit_set32(&RTC->SR, RTC_SR_TCE_SHIFT);

    return 0;
}

int rtc_get_time(struct tm *time)
{
    uint32_t t;
    for (int i = 0; i < 3; i++) {
        /* Read twice to make sure we get a stable reading */
        t = RTC->TSR;

        if (t == RTC->TSR) {
            return t;
        }
    }
    /* Fallback just return unstable reading */
    rtc_localtime(t, time);
    return 0;
}

int rtc_set_alarm(struct tm *time, rtc_alarm_cb_t cb, void *arg)
{
    uint32_t t = rtc_mktime(time);

    /* The alarm is triggered when TSR matches TAR, and TSR increments. This
     * seem counterintuitive as most users expect the alarm to trigger
     * immediately when the counter becomes equal to the alarm time.
     *
     * Workaround: Set TAF to alarm - 1
     */

    /* Disable Timer Alarm Interrupt */
    bit_clear32(&RTC->IER, RTC_IER_TAIE_SHIFT);

    RTC->TAR = t - 1;

    rtc_callback = cb;
    rtc_arg = arg;

    /* Enable Timer Alarm Interrupt */
    bit_set32(&RTC->IER, RTC_IER_TAIE_SHIFT);

    return 0;
}

int rtc_get_alarm(struct tm *time)
{
    uint32_t t =  RTC->TAR + 1;

    rtc_localtime(t, time);

    return 0;
}

void rtc_clear_alarm(void)
{
    /* Disable Timer Alarm Interrupt */
    bit_clear32(&RTC->IER, RTC_IER_TAIE_SHIFT);
    rtc_callback = NULL;
}

void rtc_poweron(void)
{
    /* Enable Time Counter */
    bit_set32(&RTC->SR, RTC_SR_TCE_SHIFT);
}

void rtc_poweroff(void)
{
    /* Disable Time Counter */
    bit_clear32(&RTC->SR, RTC_SR_TCE_SHIFT);
}

void isr_rtc(void)
{
    if (RTC->SR & RTC_SR_TAF_MASK) {
        if (rtc_callback != NULL) {
            /* Disable Timer Alarm Interrupt */
            bit_clear32(&RTC->IER, RTC_IER_TAIE_SHIFT);
            rtc_callback(rtc_arg);
        }
    }

    cortexm_isr_end();
}
