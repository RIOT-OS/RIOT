/*
 * Copyright 2013 INRIA.
 * Copyright 2014 Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_shell_commands
 * @{
 *
 * @file
 * @brief       Shell command implementation for the peripheral RTC interface
 *
 * @author  Oliver Hahm <oliver.hahm@inria.fr>
 * @author  Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>#
 *
 * @}
 */

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "fmt.h"
#include "fmt_table.h"
#include "periph/rtc.h"

static void _alarm_handler(void *arg)
{
    (void) arg;

    print_str("The alarm rang\n");
}

static int dow(int year, int month, int day)
{
    /* calculate the day of week using Tøndering's algorithm */
    static int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
    year -= month < 3;
    return (year + year/4 - year/100 + year/400 + t[month-1] + day) % 7;
}


static int _parse_time(char **argv, struct tm *time)
{
    short i;
    char *end;

    i = strtol(argv[0], &end, 10);
    time->tm_year = i - 1900;

    i = strtol(end + 1, &end, 10);
    time->tm_mon = i - 1;

    i = strtol(end + 1, &end, 10);
    time->tm_mday = i;

    i = strtol(argv[1], &end, 10);
    time->tm_hour = i;

    i = strtol(end + 1, &end, 10);
    time->tm_min = i;

    i = strtol(end + 1, &end, 10);
    time->tm_sec = i;

    time->tm_wday = dow(time->tm_year + 1900, time->tm_mon + 1, time->tm_mday);
    time->tm_isdst = -1; /* undefined */

    return 0;
}

static int _print_time(struct tm *time)
{
    print_u32_dec_zeros(time->tm_year + 1900, 4);
    print_str("-");
    print_u32_dec_zeros(time->tm_mon, 2);
    print_str("-");
    print_u32_dec_zeros(time->tm_mday, 2);
    print_str(" ");
    print_u32_dec_zeros(time->tm_hour, 2);
    print_str(":");
    print_u32_dec_zeros(time->tm_min, 2);
    print_str(":");
    print_u32_dec_zeros(time->tm_sec, 2);
    print_str("\n");
    return 0;
}

static int _rtc_getalarm(void)
{
    struct tm t;
    if (rtc_get_alarm(&t) == 0) {
        _print_time(&t);
        return 0;
    }
    else {
        print_str("rtc: error getting alarm\n");
        return 1;
    }
}

static int _rtc_setalarm(char **argv)
{
    struct tm now;

    if (_parse_time(argv, &now) == 0) {
        if (rtc_set_alarm(&now, _alarm_handler, NULL) == -1) {
            print_str("rtc: error setting alarm\n");
            return 1;
        }
        return 0;
    }
    return 1;
}

static int _rtc_gettime(void)
{
    struct tm t;
    if (rtc_get_time(&t) == 0) {
        _print_time(&t);
        return 0;
    }
    else {
        print_str("rtc: error getting time\n");
        return 1;
    }
}

static int _rtc_settime(char **argv)
{
    struct tm now;

    if (_parse_time(argv, &now) == 0) {
        if (rtc_set_time(&now) == -1) {
            print_str("rtc: error setting time\n");
            return 1;
        }
        return 0;
    }
    return 1;
}

static int _rtc_usage(void)
{
    print_str(
        "usage: rtc <command> [arguments]\n"
        "commands:\n"
        "\tpoweron\t\tpower the interface on\n"
        "\tpoweroff\tpower the interface off\n"
        "\tclearalarm\tdeactivate the current alarm\n"
        "\tgetalarm\tprint the currently alarm time\n"
        "\tsetalarm YYYY-MM-DD HH:MM:SS\n\t\t\tset an alarm for the specified time\n"
        "\tgettime\t\tprint the current time\n"
        "\tsettime YYYY-MM-DD HH:MM:SS\n\t\t\tset the current time\n"
    );
    return 0;
}

int _rtc_handler(int argc, char **argv)
{
    if (argc < 2) {
        _rtc_usage();
        return 1;
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
    else if ((strncmp(argv[1], "setalarm", 8) == 0) && (argc == 4)) {
        _rtc_setalarm(argv + 2);
    }
    else if (strncmp(argv[1], "gettime", 7) == 0) {
        _rtc_gettime();
    }
    else if ((strncmp(argv[1], "settime", 7) == 0)  && (argc == 4)) {
        _rtc_settime(argv + 2);
    }
    else {
        print_str("unknown command or missing parameters: ");
        print_str(argv[1]);
        print_str("\n\n");
        _rtc_usage();
        return 1;
    }
    return 0;
}
