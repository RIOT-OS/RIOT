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

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "container.h"
#include "periph/rtc.h"
#include "rtc_utils.h"
#include "shell.h"

static void _alarm_handler(void *arg)
{
    (void) arg;

    puts("The alarm rang");
}

/** Read a ["YYYY-MM-DD", "hh:mm:ss"] formatted value from a string array.
 *
 * This performs no validation on the entered time -- that'd be trivial on some
 * fields (month), but excessive on others (day of month -- we don't do leap
 * year calculation otherwise) and need information we don't have (leap
 * seconds) on yet others.
 *
 * Invalid inputs merely lead to out-of-range values inside the time struct.
 */
static void _parse_time(char **argv, struct tm *time)
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

    time->tm_isdst = -1; /* undefined */

    rtc_tm_normalize(time);
}

static int _print_time(struct tm *time)
{
    printf("%04i-%02i-%02i %02i:%02i:%02i\n",
            time->tm_year + 1900, time->tm_mon + 1, time->tm_mday,
            time->tm_hour, time->tm_min, time->tm_sec
          );
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
        puts("rtc: error getting alarm");
        return 1;
    }
}

static int _rtc_setalarm(char **argv)
{
    struct tm now;

    _parse_time(argv, &now);

    if (rtc_set_alarm(&now, _alarm_handler, NULL) < 0) {
        puts("rtc: error setting alarm");
        return 1;
    }

    return 0;
}

static int _rtc_gettime(void)
{
    struct tm t;
    if (rtc_get_time(&t) == 0) {
        _print_time(&t);
        return 0;
    }
    else {
        puts("rtc: error getting time");
        return 1;
    }
}

static int _rtc_settime(char **argv)
{
    struct tm now;

    _parse_time(argv, &now);

    if (RTC_SET_TIME(&now) == -1) {
        puts("rtc: error setting time");
        return 1;
    }

    return 0;
}

static int _rtc_usage(void)
{
    puts("usage: rtc <command> [arguments]");
    puts("commands:");
    puts("\tpoweron\t\tpower the interface on");
    puts("\tpoweroff\tpower the interface off");
    puts("\tclearalarm\tdeactivate the current alarm");
    puts("\tgetalarm\tprint the currently alarm time");
    puts("\tsetalarm YYYY-MM-DD HH:MM:SS\n\t\t\tset an alarm for the specified time");
    puts("\tgettime\t\tprint the current time");
    puts("\tsettime YYYY-MM-DD HH:MM:SS\n\t\t\tset the current time");
    return 0;
}

static int _rtc_handler(int argc, char **argv)
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
    else if (strncmp(argv[1], "clearalarm", 10) == 0) {
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
        printf("unknown command or missing parameters: %s\n\n", argv[1]);
        _rtc_usage();
        return 1;
    }
    return 0;
}

SHELL_COMMAND(rtc, "control RTC peripheral interface",  _rtc_handler);
