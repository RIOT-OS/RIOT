/*
 * Copyright (C) 2020 Benjamin Valentin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_periph_rtc
 * @{
 *
 * @file
 * @brief       Basic RTC implementation based on a 1 Hz clock
 *
 * @note        Unlike a real RTC, this emulated version is not guaranteed to keep
 *              time across reboots or deep sleep.
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include "periph/rtc.h"

/* In .noinit so we don't reset the counter on reboot */
static struct tm tm_now __attribute__((section(".noinit")));
static struct tm tm_alarm __attribute__((section(".noinit")));

static rtc_alarm_cb_t alarm_cb;
static void *alarm_cb_arg;

/* will be called every second */
ISR(TIMER2_OVF_vect)
{
    avr8_enter_isr();

    if (++tm_now.tm_sec > 59) {
        rtc_tm_normalize(&tm_now);
    }

    if (alarm_cb && rtc_tm_compare(&tm_now, &tm_alarm) == 0) {
        alarm_cb(alarm_cb_arg);
    }

    avr8_exit_isr();
}

void rtc_init(void)
{
    static const struct tm zero = {0};

    /* Disable all timer 2 interrupts */
    TIMSK2 = 0;

    /* avoid negative time values on init (uninitialized memory) */
    if (!rtc_tm_valid(&tm_now)) {
        tm_now = zero;
    }

    /* Select asynchronous 32 kHz clock source */
    ASSR = (1 << AS2);

    /* select normal operation */
    TCCR2A = 0x0;

    /* select divider 128 -> 1 Hz */
    TCCR2B = 0x5;

    /* Clear interrupt flags */
    TIFR2 = (1 << OCF2B) | (1 << OCF2A) | (1 << TOV2);

    /* Enable 8-bit overflow interrupt */
    TIMSK2 |= (1 << TOIE2);
}

int rtc_set_time(struct tm *time)
{
    /* second starts now */
    TCNT2 = 0;

    tm_now = *time;

    return 0;
}

int rtc_get_time(struct tm *time)
{
    uint8_t before;

    /* loop in case of overflow */
    do {
        before = TCNT2;

        /* prevent compiler from reordering memory access to tm_now,
         * including moving it out of the loop
         */
        __asm__ volatile ("" : : : "memory");

        *time = tm_now;
    } while (before > TCNT2);

    return 0;
}

int rtc_get_time_ms(struct tm *time, uint16_t *ms)
{
    uint8_t cnt_before, cnt_after;

    /* loop in case of overflow */
    do {
        cnt_before = TCNT2;

        /* prevent compiler from reordering memory access to tm_now,
         * including moving it out of the loop
         */
        __asm__ volatile ("" : : : "memory");
        *time = tm_now;

        cnt_after  = TCNT2;
    } while (cnt_before > cnt_after);

    *ms = (cnt_after * 1000UL) >> 8;

    return 0;
}

int rtc_get_alarm(struct tm *time)
{
    *time = tm_alarm;

    return 0;
}

int rtc_set_alarm(struct tm *time, rtc_alarm_cb_t cb, void *arg)
{
    rtc_tm_normalize(time);

    alarm_cb = NULL;    /* disable alarm */
    tm_alarm = *time;
    alarm_cb_arg = arg;
    alarm_cb = cb;      /* enable alarm */

    return 0;
}

void rtc_clear_alarm(void)
{
    alarm_cb = NULL;
}

void rtc_poweron(void)
{
    power_timer2_enable();
}

void rtc_poweroff(void)
{
    power_timer2_disable();
}
