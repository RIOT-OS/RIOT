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
#include "shell.h"

#include "mutex.h"
#include "periph_conf.h"
#include "periph/rtc.h"
#include "periph/rtc_mem.h"
#include "ztimer.h"

#ifdef MODULE_PERIPH_RTC_MEM
#include "periph/pm.h"
#endif

#define PERIOD              (2U)
#define REPEAT              (4U)

#define TM_YEAR_OFFSET      (1900)

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

static int _reboot(int arg, char **argv)
{
    (void)arg;
    (void)argv;
    puts("Rebooting");
    pm_reboot();
    return 0;
}
static int _clear_rtc_mem(int arg, char **argv)
{
    (void)arg;
    (void)argv;
    uint8_t pos = rtc_mem_size();
    uint8_t zero = 0;

    while (pos--) {
        rtc_mem_write(zero, &zero, sizeof(zero));
    }
    puts("RTC mem cleared");
    return 0;
}

static int _set_rtc_mem(int arg, char **argv)
{
    (void)arg;
    (void)argv;
    /* first fill the whole memory */
    uint8_t size = rtc_mem_size();

    while (size--) {
        rtc_mem_write(size, &size, sizeof(size));
    }

    /* write test data */
    rtc_mem_write(riot_msg_offset, riot_msg, sizeof(riot_msg) - 1);
    puts("RTC mem set");
    return 0;
}

static int _verify_rtc_mem(int arg, char **argv)
{
    (void)arg;
    (void)argv;
    char buf[4];

    rtc_mem_read(riot_msg_offset, buf, sizeof(buf));

    if (memcmp(buf, riot_msg, sizeof(buf))) {
        puts("RTC mem content does not match");
        for (unsigned i = 0; i < sizeof(buf); ++i) {
            printf("%02x - %02x\n", riot_msg[i], buf[i]);
        }
        return -1;
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
    return 0;
}
#endif
static mutex_t rtc_mtx = MUTEX_INIT_LOCKED;
static int _test_alarms(int arg, char **argv)
{
    (void)arg;
    (void)argv;

    printf("This test will display 'Alarm!' every %u seconds for %u times\n",
           PERIOD, REPEAT);

    struct tm time = (struct tm){ 0 };
    uint16_t ms;

/* read RTC to retrieve initial */
    if (IS_USED(MODULE_PERIPH_RTC_MS)) {
        rtc_get_time_ms(&time, &ms);
        print_time_ms("Clock value is now ", &time, ms);
    }
    else {
        rtc_get_time(&time);
        print_time("Clock value is now ", &time);
    }

    time = (struct tm){
        .tm_year = 2020 - TM_YEAR_OFFSET,   /* years are counted from 1900 */
        .tm_mon =  1,                       /* 0 = January, 11 = December */
        .tm_mday = 28,
        .tm_hour = 23,
        .tm_min = 59,
        .tm_sec = 57
    };

    /* set RTC */
    print_time("  Setting clock to ", &time);
    rtc_set_time(&time);

    time = (struct tm){ 0 };
    ms = 0;

    /* read RTC to confirm value */
    if (IS_USED(MODULE_PERIPH_RTC_MS)) {
        rtc_get_time_ms(&time, &ms);
        print_time_ms("Clock value is now ", &time, ms);
    }
    else {
        rtc_get_time(&time);
        print_time("Clock value is now ", &time);
    }

    /* set initial alarm */
    inc_secs(&time, PERIOD);
    print_time("  Setting alarm to ", &time);
    rtc_set_alarm(&time, cb, &rtc_mtx);

    time = (struct tm){ 0 };
    ms = 0;

    /* verify alarm */
    rtc_get_alarm(&time);
    print_time("   Alarm is set to ", &time);

    time = (struct tm){ 0 };
    ms = 0;

    /* clear alarm */
    rtc_clear_alarm();
    if (IS_USED(MODULE_PERIPH_RTC_MS)) {
        rtc_get_time_ms(&time, &ms);
        print_time_ms("  Alarm cleared at ", &time, ms);
    }
    else {
        rtc_get_time(&time);
        print_time("  Alarm cleared at ", &time);
    }

    /* verify alarm has been cleared */
    ztimer_sleep(ZTIMER_MSEC, PERIOD*1000);

    const char *message;

    if (mutex_trylock(&rtc_mtx)) {
        message = "   Error: Alarm at ";
    }
    else {
        message = "       No alarm at ";
    }

    time = (struct tm){ 0 };
    ms = 0;

    if (IS_USED(MODULE_PERIPH_RTC_MS)) {
        rtc_get_time_ms(&time, &ms);
        print_time_ms(message, &time, ms);
    }
    else {
        rtc_get_time(&time);
        print_time(message, &time);
    }

    time = (struct tm){ 0 };
    ms = 0;

    /* set alarm */
    rtc_get_time(&time);
    inc_secs(&time, PERIOD);
    rtc_set_alarm(&time, cb, &rtc_mtx);
    print_time("  Setting alarm to ", &time);
    puts("");

    /* loop over a few alarm cycles */
    unsigned int cnt = 0;

    do {
        mutex_lock(&rtc_mtx);
        puts("Alarm!");

        struct tm time = { 0 };
        rtc_get_alarm(&time);
        inc_secs(&time, PERIOD);
        rtc_set_alarm(&time, cb, &rtc_mtx);
    } while (++cnt < REPEAT);
    return 0;
}

static const shell_command_t commands[] = {
#ifdef MODULE_PERIPH_RTC_MEM
    { "rtc_reboot", "reboots the MCU", _reboot },
    { "rtc_clear", "clears the RTC memory", _clear_rtc_mem },
    { "rtc_verify", "verifies the RTC memory", _verify_rtc_mem },
    { "rtc_write", "writes to the RTC memory", _set_rtc_mem },
#endif
    { "rtc_test_alarms", "tests RTC alarms", _test_alarms },
    { NULL, NULL, NULL }
};

int main(void)
{

    puts("");
    puts("RIOT RTC low-level driver test");
    puts("Write help to see available commands");

    rtc_init();
    char line_buf[SHELL_DEFAULT_BUFSIZE];

    shell_run(commands, line_buf, SHELL_DEFAULT_BUFSIZE);
    return 0;
}
