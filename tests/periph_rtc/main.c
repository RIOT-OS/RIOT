/*
 * Copyright (C) 2015 Lari Lehtomäki
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test for low-level Real Time clock drivers
 *
 * This test will initialize the real-time timer and trigger an alarm printing
 * 'Hello' every 10 seconds
 *
 * @author      Lari Lehtomäki <lari@lehtomaki.fi>
 *
 * @}
 */

#include <stdio.h>
#include <time.h>

#include "mutex.h"
#include "periph_conf.h"
#include "periph/rtc.h"
#include "xtimer.h"

#define PERIOD              (2U)
#define REPEAT              (4U)

#define TM_YEAR_OFFSET      (1900)

static unsigned cnt = 0;
static uint32_t start_usecs = 0;
static uint32_t alarm_expected_usecs = 0;

static void print_time(const char *label, const struct tm *time)
{
    printf("%s  %04d-%02d-%02d %02d:%02d:%02d\n", label,
            time->tm_year + TM_YEAR_OFFSET,
            time->tm_mon + 1,
            time->tm_mday,
            time->tm_hour,
            time->tm_min,
            time->tm_sec);
}

static void print_time_ms(const char *label, const struct tm *time, uint16_t ms)
{
    printf("%s  %04d-%02d-%02d %02d:%02d:%02d.%03d\n", label,
            time->tm_year + TM_YEAR_OFFSET,
            time->tm_mon + 1,
            time->tm_mday,
            time->tm_hour,
            time->tm_min,
            time->tm_sec,
            ms);
}

static void cb(void *arg)
{
    mutex_unlock(arg);
}

static void rtc_wait(unsigned secs)
{
    struct tm time;
    mutex_t rtc_mtx = MUTEX_INIT_LOCKED;

    rtc_get_time(&time);
    time.tm_sec += secs;
    rtc_set_alarm(&time, cb, &rtc_mtx);
    mutex_lock(&rtc_mtx);
}

int main(void)
{
    struct tm time = {
        .tm_year = 2020 - TM_YEAR_OFFSET,   /* years are counted from 1900 */
        .tm_mon  =  1,                      /* 0 = January, 11 = December */
        .tm_mday = 28,
        .tm_hour = 23,
        .tm_min  = 59,
        .tm_sec  = 57
    };

    mutex_t rtc_mtx = MUTEX_INIT_LOCKED;

    puts("\nRIOT RTC low-level driver test");
    printf("This test will display 'Alarm!' every %u seconds for %u times\n",
            PERIOD, REPEAT);

    rtc_init();

    /* set RTC */
    print_time("  Setting clock to ", &time);
    rtc_set_time(&time);

    /* read RTC to confirm value */
    if (IS_USED(MODULE_PERIPH_RTC_MS)) {
        uint16_t ms;
        rtc_get_time_ms(&time, &ms);
        print_time_ms("Clock value is now ", &time, ms);
    } else {
        rtc_get_time(&time);
        print_time("Clock value is now ", &time);
    }

    /* set initial alarm */
    time.tm_sec += PERIOD;
    print_time("  Setting alarm to ", &time);
    rtc_set_alarm(&time, cb, &rtc_mtx);

    /* verify alarm */
    rtc_get_alarm(&time);
    print_time("   Alarm is set to ", &time);

    /* clear alarm */
    rtc_clear_alarm();
    if (IS_USED(MODULE_PERIPH_RTC_MS)) {
        uint16_t ms;
        rtc_get_time_ms(&time, &ms);
        print_time_ms("  Alarm cleared at ", &time, ms);
    } else {
        rtc_get_time(&time);
        print_time("  Alarm cleared at ", &time);
    }

    /* verify alarm has been cleared */
    xtimer_sleep(PERIOD);

    const char *message;
    if (mutex_trylock(&rtc_mtx)) {
        message = "   Error: Alarm at ";
    } else {
        message = "       No alarm at ";
    }

    if (IS_USED(MODULE_PERIPH_RTC_MS)) {
        uint16_t ms;
        rtc_get_time_ms(&time, &ms);
        print_time_ms(message, &time, ms);
    } else {
        rtc_get_time(&time);
        print_time(message, &time);
    }

    /* sync to start of second */
    rtc_wait(1);

    /* set alarm */
    rtc_get_time(&time);
    start_usecs = xtimer_now_usec();
    alarm_expected_usecs = start_usecs + US_PER_SEC * PERIOD;
    time.tm_sec += PERIOD;
    rtc_set_alarm(&time, cb, &rtc_mtx);
    print_time("  Setting alarm to ", &time);
    puts("");

    printf("[%" PRIu32 "] First alarm expected at %" PRIu32 " µs\n",
           xtimer_now_usec(),
           alarm_expected_usecs);

    /* loop over a few alarm cycles */
    while (1) {
        mutex_lock(&rtc_mtx);

        /* capture time before printf */
        uint32_t now_usecs = xtimer_now_usec();
        if (++cnt < REPEAT) {
            struct tm alarm = time;
            rtc_get_time(&time);

            if (alarm.tm_sec != time.tm_sec) {
                puts("alarm time mismatch");
                print_time("expected: ", &alarm);
                print_time("now:      ", &time);
            }

            time.tm_sec += PERIOD;
            rtc_set_alarm(&time, cb, &rtc_mtx);
        }

        printf("[%" PRIu32 "] Alarm! after %" PRIu32 " µs "
               "(error %" PRId32 " µs)\n",
               now_usecs,
               now_usecs - start_usecs,
               (int32_t)now_usecs - alarm_expected_usecs);

        alarm_expected_usecs = now_usecs + US_PER_SEC * PERIOD;
    }

    return 0;
}
