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

#include "periph_conf.h"
#include "periph/rtc.h"
#include "xtimer.h"
#include "msg.h"
#include "thread.h"

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
    if (time->tm_sec >= 60) {
        time->tm_sec -= 60;
    }
}

static void cb(void *arg)
{
    msg_t msg;
    msg_send(&msg, *(kernel_pid_t *)arg);
}

int main(void)
{
    struct tm time = {
        .tm_year = 2011 - TM_YEAR_OFFSET,   /* years are counted from 1900 */
        .tm_mon  = 11,                      /* 0 = January, 11 = December */
        .tm_mday = 13,
        .tm_hour = 14,
        .tm_min  = 15,
        .tm_sec  = 15
    };

    puts("\nRIOT RTC low-level driver test");
    printf("This test will display 'Alarm!' every %u seconds for %u times\n",
            PERIOD, REPEAT);

    /* set RTC */
    print_time("  Setting clock to ", &time);
    rtc_set_time(&time);

    /* read RTC to confirm value */
    rtc_get_time(&time);
    print_time("Clock value is now ", &time);

    /* set initial alarm */
    inc_secs(&time, PERIOD);
    print_time("  Setting alarm to ", &time);

    kernel_pid_t p_main = sched_active_pid;
    rtc_set_alarm(&time, cb, &p_main); 

    /* verify alarm */
    rtc_get_alarm(&time);
    print_time("   Alarm is set to ", &time);
    puts("");

    msg_t msg_req;
    while (cnt++ < REPEAT) {
        msg_receive(&msg_req);
        puts("Alarm!");

        struct tm time;
        rtc_get_time(&time);
        inc_secs(&time, PERIOD);
        rtc_set_alarm(&time, cb, &p_main);
    }

    puts("[SUCCESS]");

    return 0;
}
