/**
 * Shell commands for peripheral interfaces
 *
 * Copyright 2014  Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup shell_commands
 * @{
 * @file
 * @brief   provides shell commands for low level peripheral
 *          interfaces
 * @author  Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 * @}
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "periph_conf.h"

#if RTC_NUMOF
#include "periph/rtc.h"

void _alarm_handler(void *arg)
{
    (void) arg;

    puts("The alarm rang");
}

static int _parse_time(char **argv, struct tm *time)
{
    short i1, i2, i3;

    if(sscanf(argv[0], "%6hd-%6hd-%6hd", &i1, &i2, &i3) != 3) {
        puts("could not parse date");
        return -1;
    }

    time->tm_year = i1 - 1900;
    time->tm_mon = i2 - 1;
    time->tm_mday = i3;

    if(sscanf(argv[1], "%6hd:%6hd:%6hd", &i1, &i2, &i3) != 3) {
        puts("could not parse time");
        return -1;
    }

    time->tm_hour = i1;
    time->tm_min = i2;
    time->tm_sec = i3;

    return 0;
}

static void _rtc_getalarm(void)
{
    struct tm now;
    if (rtc_get_alarm(&now) == 0) {
        /* cppcheck: see man 3 asctime: obsoleted by POSIX.1-2008 */
        /* cppcheck-suppress obsoleteFunctionsasctime */
        printf("%s", asctime(&now));
    }
    else {
        puts("error");
    }
}

static void _rtc_setalarm(char **argv)
{
    struct tm now;

    if (_parse_time(argv, &now) == 0) {
        if (rtc_set_alarm(&now, _alarm_handler, NULL) == -1) {
            puts("error");
        }
    }
}

static void _rtc_gettime(void)
{
    struct tm now;
    rtc_get_time(&now);

    /* cppcheck: see man 3 asctime: obsoleted by POSIX.1-2008 */
    /* cppcheck-suppress obsoleteFunctionsasctime */
    printf("%s", asctime(&now));
}

static void _rtc_settime(char **argv)
{
    struct tm now;

    if (_parse_time(argv, &now) == 0) {
        rtc_set_time(&now);
    }
}

static void _rtc_usage(void)
{
    puts("usage: rtc <command> [arguments]");
    puts("commands:");
    puts("\tinit\t\tinitialize the interface");
    puts("\tpoweron\t\tpower the interface on");
    puts("\tpoweroff\tpower the interface off");
    puts("\tclearalarm\tdeactivate the current alarm");
    puts("\tgetalarm\tprint the currently alarm time");
    puts("\tsetalarm YYYY-MM-DD HH:MM:SS\n\t\t\tset an alarm for the specified time");
    puts("\tgettime\t\tprint the current time");
    puts("\tsettime YYYY-MM-DD HH:MM:SS\n\t\t\tset the current time");
}

void _rtc_handler(int argc, char **argv)
{
    if (argc < 2) {
        _rtc_usage();
    }
    else if (strncmp(argv[1], "init", 4) == 0) {
        rtc_init();
    }
    else if (strncmp(argv[1], "poweron", 7) == 0) {
        rtc_poweron();
    }
    else if (strncmp(argv[1], "poweroff", 8) == 0) {
        rtc_poweroff();
    }
    else if (strncmp(argv[1], "clearalarm", 8) == 0) {
        rtc_clear_alarm();
    }
    else if (strncmp(argv[1], "getalarm", 8) == 0) {
        _rtc_getalarm();
    }
    else if (strncmp(argv[1], "setalarm", 8) == 0) {
        _rtc_setalarm(argv + 2);
    }
    else if (strncmp(argv[1], "gettime", 7) == 0) {
        _rtc_gettime();
    }
    else if (strncmp(argv[1], "settime", 7) == 0) {
        _rtc_settime(argv + 2);
    }
    else {
        printf("unknown command: %s\n", argv[1]);
    }
}

#endif /* RTC_NUMOF */

#if RANDOM_NUMOF
#include "periph/random.h"

static void _random_get(int argc, char **argv)
{
    unsigned count = 1;

    if (argc == 1) {
        if (sscanf(argv[0], "%6u", &count) != 1) {
            puts("error scanning count");
            return;
        }
    }

    char x;
    while (count--) {
        if (random_read(&x, 1)){
            printf(" %02x", (unsigned char)x);
        }
        else {
            printf("error reading data");
        }
    }
    puts("");
}

static void _random_usage(void)
{
    puts("usage: random <command> [arguments]");
    puts("commands:");
    puts("\tinit\t\tinitialize the interface");
    puts("\tpoweron\t\tpower the interface on");
    puts("\tpoweroff\tpower the interface off");
    puts("\tget [count]\tprint count (default: 1) random bytes");
}

void _random_handler(int argc, char **argv)
{
    if (argc < 2) {
        _random_usage();
    }
    else if (strncmp(argv[1], "init", 4) == 0) {
        random_init();
    }
    else if (strncmp(argv[1], "poweron", 7) == 0) {
        random_poweron();
    }
    else if (strncmp(argv[1], "poweroff", 8) == 0) {
        random_poweroff();
    }
    else if (strncmp(argv[1], "get", 3) == 0) {
        _random_get(argc - 2, argv + 2);
    }
    else {
        printf("unknown command: %s\n", argv[1]);
    }
}
#endif
