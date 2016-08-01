/*
 * Copyright 2013 INRIA.
 * Copyright 2014 Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 * Copyright 2016 Eistec AB
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
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "rtc.h"

static void _alarm_handler(void *arg)
{
    (void) arg;

    puts("The alarm rang");
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
    printf("%04i-%02i-%02i %02i:%02i:%02i\n",
            time->tm_year + 1900, time->tm_mon + 1, time->tm_mday,
            time->tm_hour, time->tm_min, time->tm_sec
          );
    return 0;
}

static int _rtc_getalarm(rtc_t *rtc)
{
    struct tm target;
    int err = rtc_get_alarm(rtc, &target);
    if (err != 0) {
        printf("rtc: error %d getting alarm\n", err);
        return 1;
    }
    _print_time(&target);
    return 0;
}

static int _rtc_setalarm(rtc_t *rtc, char **argv)
{
    struct tm target;

    if (_parse_time(argv, &target) != 0) {
        puts("rtc: error parsing time");
        return 1;
    }
    int err = rtc_set_alarm(rtc, &target, _alarm_handler, NULL);
    if (err != 0) {
        printf("rtc: error %d setting alarm\n", err);
        return 1;
    }
    return 0;
}

static int _rtc_gettime(rtc_t *rtc)
{
    struct tm now;
    int err = rtc_get_time(rtc, &now);
    if (err != 0) {
        printf("rtc: error %d getting time\n", err);
        return 1;
    }
    _print_time(&now);
    return 0;
}

static int _rtc_settime(rtc_t *rtc, char **argv)
{
    struct tm now;

    if (_parse_time(argv, &now) != 0) {
        puts("rtc: error parsing time");
        return 1;
    }
    _print_time(&now);
    int err = rtc_set_time(rtc, &now);
    if (err != 0) {
        printf("rtc: error %d setting time\n", err);
        return 1;
    }
    return 0;
}

static int _rtc_usage(void)
{
    puts("usage: rtc <command> [arguments]");
    puts("commands:");
    puts("\tlist\t\tlist all registered RTCs");
    puts("\tinit\t\tinitialize the interface");
    puts("\tclearalarm\tdeactivate the current alarm");
    puts("\tgetalarm\tprint the currently alarm time");
    puts("\tsetalarm YYYY-MM-DD HH:MM:SS\n\t\t\tset an alarm for the specified time");
    puts("\tgettime\t\tprint the current time");
    puts("\tsettime YYYY-MM-DD HH:MM:SS\n\t\t\tset the current time");
    return 0;
}

void _rtc_list(void) {
    rtc_t *rtc = rtc_iterate(NULL); /* Get first device */
    if (rtc == NULL) {
        /* No RTCs */
        puts("No RTCs registered");
        return;
    }
    puts("These are the currently registered RTC devices:");
    while(rtc != NULL) {
        printf("%s\n", rtc->name); /* newlib will print (nil) if name is NULL */
        rtc = rtc_iterate(rtc);
    }
}

int _rtc_handler(int argc, char **argv)
{
    if (argc < 2) {
        _rtc_usage();
        return 1;
    }
    const char *cmd = argv[1];
    if (strcmp(cmd, "list") == 0) {
        _rtc_list();
        return 0;
    }

    if (argc < 3) {
        _rtc_usage();
        return 1;
    }

    const char *name = argv[2];
    rtc_t *rtc = rtc_find(name);
    if (rtc == NULL) {
        printf("No RTC by that name: %s\n", name);
        return 1;
    }

    if (strcmp(cmd, "init") == 0) {
        rtc_init(rtc);
    }
    else if (strcmp(cmd, "clearalarm") == 0) {
        rtc_clear_alarm(rtc);
    }
    else if (strcmp(cmd, "getalarm") == 0) {
        _rtc_getalarm(rtc);
    }
    else if (strcmp(cmd, "setalarm") == 0) {
        _rtc_setalarm(rtc, argv + 3);
    }
    else if (strcmp(cmd, "gettime") == 0) {
        _rtc_gettime(rtc);
    }
    else if (strcmp(cmd, "settime") == 0) {
        _rtc_settime(rtc, argv + 3);
    }
    else {
        printf("unknown command: %s\n", cmd);
        return 1;
    }
    return 0;
}
