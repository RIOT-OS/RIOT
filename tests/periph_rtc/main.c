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

static void inc_secs(struct tm *time, unsigned val)
{
    time->tm_sec += val;
}

static void cb(void *arg)
{
    mutex_unlock(arg);
}

int main(void)
{
    struct tm time = {
        .tm_year = 2011 - TM_YEAR_OFFSET,   /* years are counted from 1900 */
        .tm_mon  = 11,                      /* 0 = January, 11 = December */
        .tm_mday = 13,
        .tm_hour = 14,
        .tm_min  = 15,
        .tm_sec  = 57
    };

    mutex_t rtc_mtx = MUTEX_INIT_LOCKED;

    puts("\nRIOT RTC low-level driver test");
    printf("This test will display 'Alarm!' every %u seconds for %u times\n",
            PERIOD, REPEAT);

    /* set RTC */
    print_time("  Setting clock to ", &time);
    rtc_set_time(&time);
    start_usecs = xtimer_now_usec();

    /* read RTC to confirm value */
    struct tm time_now;
    rtc_get_time(&time_now);
    print_time("Clock value is now ", &time_now);

    /* set initial alarm */
    inc_secs(&time, PERIOD);
    print_time("  Setting alarm to ", &time);
    rtc_set_alarm(&time, cb, &rtc_mtx);
    alarm_expected_usecs = start_usecs + US_PER_SEC * PERIOD;

    /* verify alarm */
    rtc_get_alarm(&time);
    print_time("   Alarm is set to ", &time);
    puts("");

    printf("[%" PRIu32 "] First alarm expected at %" PRIu32 " microseconds\n",
    xtimer_now_usec(),
    alarm_expected_usecs);
    while (1) {
        mutex_lock(&rtc_mtx);
        uint32_t now_usecs = xtimer_now_usec();
        printf("[%" PRIu32 "] Alarm! after %" PRIu32 " microseconds (error %" PRId32 " microseconds)\n",
           now_usecs,
           now_usecs - start_usecs,
           (int32_t)now_usecs - alarm_expected_usecs);
        if (++cnt < REPEAT) {
            struct tm time;
            rtc_get_alarm(&time);
            inc_secs(&time, PERIOD);
            rtc_set_alarm(&time, cb, &rtc_mtx);
            alarm_expected_usecs += US_PER_SEC * PERIOD;
        }
    }

    return 0;
}
