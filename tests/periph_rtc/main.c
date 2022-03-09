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
#include <string.h>

#include "mutex.h"
#include "periph_conf.h"
#include "periph/rtc.h"
#include "periph/rtc_mem.h"
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

static void inc_secs(struct tm *time, unsigned val)
{
    time->tm_sec += val;
}

static void cb(void *arg)
{
    mutex_unlock(arg);
}

#ifdef MODULE_PERIPH_RTC_MEM
static const uint8_t riot_msg_offset = 1;
static const char riot_msg[] = "RIOT";
static void _set_rtc_mem(void)
{
    /* first fill the whole memory */
    uint8_t size = rtc_mem_size();
    while (size--) {
        rtc_mem_write(size, &size, sizeof(size));
    }

    /* write test data */
    rtc_mem_write(riot_msg_offset, riot_msg, sizeof(riot_msg) - 1);
}

static void _get_rtc_mem(void)
{
    char buf[4];
    rtc_mem_read(riot_msg_offset, buf, sizeof(buf));

    if (memcmp(buf, riot_msg, sizeof(buf))) {
        puts("RTC mem content does not match");
        for (unsigned i = 0; i < sizeof(buf); ++i) {
            printf("%02x - %02x\n", riot_msg[i], buf[i]);
        }
        return;
    }

    uint8_t size = rtc_mem_size();
    while (size--) {
        uint8_t data;

        if (size >= riot_msg_offset &&
            size < riot_msg_offset + sizeof(riot_msg)) {
            continue;
        }

        rtc_mem_read(size, &data, 1);
        if (data != size) {
            puts("RTC mem content does not match");
            printf("%02x: %02x\n", size, data);
        }
    }


    puts("RTC mem OK");
}
#else
static inline void _set_rtc_mem(void) {}
static inline void _get_rtc_mem(void) {}
#endif

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

    _set_rtc_mem();
    _get_rtc_mem();

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
    inc_secs(&time, PERIOD);
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

    /* set alarm */
    rtc_get_time(&time);
    inc_secs(&time, PERIOD);
    rtc_set_alarm(&time, cb, &rtc_mtx);
    print_time("  Setting alarm to ", &time);
    puts("");

    /* loop over a few alarm cycles */
    do {
        mutex_lock(&rtc_mtx);
        puts("Alarm!");

        struct tm time;
        rtc_get_alarm(&time);
        inc_secs(&time, PERIOD);
        rtc_set_alarm(&time, cb, &rtc_mtx);
    } while (++cnt < REPEAT);

    _get_rtc_mem();

    return 0;
}
