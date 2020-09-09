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
    rtc_get_time(&time);
    print_time("Clock value is now ", &time);

    /* set initial alarm */
    inc_secs(&time, PERIOD);
    print_time("  Setting alarm to ", &time);
    rtc_set_alarm(&time, cb, &rtc_mtx);

    /* verify alarm */
    rtc_get_alarm(&time);
    print_time("   Alarm is set to ", &time);
    puts("");

    while (1) {
        mutex_lock(&rtc_mtx);
        puts("Alarm!");

        if (++cnt < REPEAT) {
            struct tm time;
            rtc_get_alarm(&time);
            inc_secs(&time, PERIOD);
            rtc_set_alarm(&time, cb, &rtc_mtx);
        }
    }

    return 0;
}
