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

#define TM_YEAR_OFFSET              (1900)

#if RTC_NUMOF < 1
#error "No RTC found. See the board specific periph_conf.h."
#endif

void cb(void *arg)
{
    (void)arg;

    puts("Alarm!");

    struct tm time;
    rtc_get_alarm(&time);
    time.tm_sec  += 10;
    if ( time.tm_sec > 60 )
        rtc_clear_alarm();
    rtc_set_alarm(&time, cb, 0);
}


int main(void)
{
    puts("\nRIOT RTC low-level driver test");
    puts("This test will display 'Alarm' in 10 seconds\n");

    puts("Initializing the RTC driver");
    rtc_init();

    struct tm time;
    time.tm_year = 2011 - TM_YEAR_OFFSET; // years are counted from 1900
    time.tm_mon  = 11; // 0 = January, 11 = December
    time.tm_mday = 13;
    time.tm_hour = 14;
    time.tm_min  = 15;
    time.tm_sec  = 15;

    printf("Setting clock to %04d-%02d-%02d %02d:%02d:%02d\n",
                                                    time.tm_year + TM_YEAR_OFFSET,
                                                    time.tm_mon + 1,
                                                    time.tm_mday,
                                                    time.tm_hour,
                                                    time.tm_min,
                                                    time.tm_sec);
    rtc_set_time(&time);

    xtimer_usleep(100);

    rtc_get_time(&time);
    printf("Clock set to %04d-%02d-%02d %02d:%02d:%02d\n",
                                                    time.tm_year + TM_YEAR_OFFSET,
                                                    time.tm_mon + 1,
                                                    time.tm_mday,
                                                    time.tm_hour,
                                                    time.tm_min,
                                                    time.tm_sec);


    time.tm_sec  += 10;

    printf("Setting alarm to %04d-%02d-%02d %02d:%02d:%02d\n",
                                                    time.tm_year + TM_YEAR_OFFSET,
                                                    time.tm_mon + 1,
                                                    time.tm_mday,
                                                    time.tm_hour,
                                                    time.tm_min,
                                                    time.tm_sec);
    rtc_set_alarm(&time, cb, 0);

    xtimer_usleep(100);

    rtc_get_alarm(&time);
    printf("Alarm set to %04d-%02d-%02d %02d:%02d:%02d\n",
                                                    time.tm_year + TM_YEAR_OFFSET,
                                                    time.tm_mon + 1,
                                                    time.tm_mday,
                                                    time.tm_hour,
                                                    time.tm_min,
                                                    time.tm_sec);

    puts("The alarm should trigger every 10 seconds for 4 times.");
    return 0;
}
