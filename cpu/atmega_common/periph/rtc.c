/*
 * Copyright (C) 2018 Acutam Automation, LLC
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_atmega_common
 * @ingroup     drivers_periph_rtc
 *
 * @{
 *
 * @file
 * @brief       RTC interface wrapper for use with RTT modules.
 *
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 *
 * In order to safely sleep when using the RTT:
 * 1. Disable interrupts
 * 2. Write to one of the asynch registers (e.g. TCCR2A)
 * 3. Wait for ASSR register's busy flags to clear
 * 4. Re-enable interrupts
 * 5. Sleep before interrupt re-enable takes effect
 *
 * @}
 */

#include <avr/interrupt.h>
#include <time.h>

#include "cpu.h"
#include "irq.h"
#include "periph/rtc.h"
#include "periph/rtt.h"
#include "periph_conf.h"
#include "thread.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/* guard file in case no RTC device is defined */
#if RTC_NUMOF

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    time_t alarm;               /* alarm_cb when time == alarm */
    rtc_alarm_cb_t alarm_cb;    /* callback called from RTC alarm */
    void *alarm_arg;            /* argument passed to the callback */
} rtc_state_t;

static rtc_state_t rtc_state;

/* RTC second counter. This is treated as-if it is a register as it is
 * incremented by the overflow ISR. This MUST BE marked volatile for two
 * reasons: to guarantee ordering with other register operations, and to
 * force redundant access to occur (in case it was changed by the ISR). */
static volatile time_t time_cnt;

static inline void _asynch_wait(void)
{
    /* Wait until all busy flags clear. According to the datasheet,
     * this can take up to 2 positive edges of TOSC1 (32kHz). */
    while (ASSR & ((1 << TCN2UB) | (1 << OCR2AUB) | (1 << OCR2BUB)
                   | (1 << TCR2AUB) | (1 << TCR2BUB))) {}
}

/* This safely reads time_cnt and TCNT2, since an atomic read cannot be provided.
 * Note: still requires prior synch using _asynch_wait() */
static inline time_t _safe_time_get(void)
{
    time_t time_tmp;
    uint8_t irq_flag, cnt_tmp;

    do {
        /* Operations must occur in this order: */
        irq_flag = TIFR2 & (1 << TOV2);
        time_tmp = time_cnt;
        cnt_tmp = TCNT2;

    /* Detect rollover after time_tmp read. If condition is met, then rollover
     * just occured, and it is safe to read again now. */
    } while ((time_tmp != time_cnt) || (irq_flag != (TIFR2 & (1 << TOV2))));

    if (irq_flag) {
        time_tmp += 8;
    }

    return time_tmp + (cnt_tmp >> 5);
}

void rtc_init(void)
{
    /* RTC depends on RTT */
    rtt_init();
}

int rtc_set_time(struct tm *time)
{
    uint8_t irq_flag;

    /* Make sure it is safe to read TCNT2, in case we just woke up */
    DEBUG("RTT sleeps until safe to read TCNT2\n");
    TCCR2A = 0;
    _asynch_wait();

    /* Convert to seconds since the epoch */
    time_t time_secs = mk_gmtime(time);

    /* Make non-atomic writes atomic */
    unsigned state = irq_disable();

    do {
        /* Operations must occur in this order: */
        time_cnt = time_secs - (TCNT2 >> 5);
        irq_flag = TIFR2 & (1 << TOV2);

    /* Detect rollover after time_cnt is written. If condition is met, then
     * rollover just occured, and it is safe to write now. */
    } while (irq_flag != (TIFR2 & (1 << TOV2)));

    irq_restore(state);

    DEBUG("RTC set time: %" PRIu32 " seconds\n", time_cnt);

    return 0;
}

int rtc_get_time(struct tm *time)
{
    /* Make sure it is safe to read TCNT2, in case we just woke up */
    DEBUG("RTT sleeps until safe to read TCNT2\n");
    TCCR2A = 0;
    _asynch_wait();

    time_t time_secs = _safe_time_get();

    /* Convert from seconds since the epoch */
    gmtime_r(&time_secs, time);

    DEBUG("RTC get time: %" PRIu32 " seconds\n", time_secs);

    return 0;
}

int rtc_set_alarm(struct tm *time, rtc_alarm_cb_t cb, void *arg)
{
    /* Disable alarm */
    rtc_clear_alarm();

    /* Make sure it is safe to read TCNT2, in case we just woke up, and */
    /* safe to write OCR2B (in case it was busy) */
    DEBUG("RTC sleeps until safe read TCNT2 and to write OCR2B\n");
    TCCR2A = 0;
    _asynch_wait();

    time_t now = _safe_time_get();
    time_t alarm_tmp = mk_gmtime(time);

    if (alarm_tmp < now) {
        DEBUG("RTC alarm set in the past. Time: %" PRIu32 " seconds, alarm: %"
              PRIu32 "\n", now, rtc_state.alarm);
        cb(arg);
        return 0;
    }

    /* Calculate time to wait, based on TCNT2 = 0 */
    time_t diff = rtc_state.alarm - now;

    /* Make non-atomic writes atomic */
    unsigned state = irq_disable();

    /* Set alarm time */
    rtc_state.alarm = alarm_tmp;

    /* Prepare the counter for sub 8-second precision */
    OCR2B = (uint8_t)(diff << 5);

    rtc_state.alarm_arg = arg;
    rtc_state.alarm_cb = cb;

    irq_restore(state);

    DEBUG("RTC set alarm: %" PRIu32 " seconds, OCR2B: %" PRIu8 "\n",
          rtc_state.alarm, OCR2B);

    /* Enable irq only if alarm is in the 8s period before it overflows */
    if (diff < 8) {
        /* Clear interrupt flag */
        TIFR2 = (1 << OCF2B);

        /* Enable interrupt */
        TIMSK2 |= (1 << OCIE2B);

        DEBUG("RTT alarm interrupt active\n");
    }
    else {
        DEBUG("RTT alarm interrupt not active\n");
    }

    return 0;
}

int rtc_get_alarm(struct tm *time)
{
    /* Convert from seconds since the epoch */
    gmtime_r(&rtc_state.alarm, time);

    DEBUG("RTC get alarm: %" PRIu32 " seconds\n", alarm);

    return 0;
}

void rtc_clear_alarm(void)
{
    /* Make non-atomic writes atomic */
    unsigned state = irq_disable();

    /* Disable alarm interrupt */
    TIMSK2 &= ~(1 << OCIE2B);

    rtc_state.alarm_cb = NULL;
    rtc_state.alarm_arg = NULL;

    irq_disable(state);
}

void rtc_poweron(void)
{
    rtt_poweron();
}

void rtc_poweroff(void)
{
    rtt_poweroff();
}

void atmega_rtc_incr(void)
{
    time_cnt += 8;

    /* If alarm not set, nothing else to do */
    if (rtc_state.alarm_cb == NULL) {
        return;
    }

    /* Check to see if alarm was missed */
    if (rtc_state.alarm <= time_cnt) {
        /* Clear callback */
        rtc_alarm_cb_t cb = rtc_state.alarm_cb;
        rtc_state.alarm_cb = NULL;

        /* Execute callback */
        cb(rtc_state.alarm_arg);

        return;
    }

    /* Enable irq only if alarm is in the 8s period before it overflows */
    if ((rtc_state.alarm - time_cnt) < 8) {
        /* Clear interrupt flag */
        TIFR2 = (1 << OCF2B);

        /* Enable interrupt */
        TIMSK2 |= (1 << OCIE2B);
    }
}

ISR(TIMER2_COMPB_vect)
{
    __enter_isr();
    /* Disable alarm interrupt */
    TIMSK2 &= ~(1 << OCIE2B);

    /* Execute callback */
    if (rtc_state.alarm_cb != NULL) {
        /* Clear callback */
        rtc_alarm_cb_t cb = rtc_state.alarm_cb;
        rtc_state.alarm_cb = NULL;

        /* Execute callback */
        cb(rtc_state.alarm_arg);
    }

    __exit_isr();
}

#ifdef __cplusplus
}
#endif

#endif /* RTC_NUMOF */
