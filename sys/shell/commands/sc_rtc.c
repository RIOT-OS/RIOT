/**
 * Shell commands for real time clock
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup shell_commands
 * @{
 * @file    sc_rtc.c
 * @brief   provides shell commands to access the rtc
 * @author  Oliver Hahm <oliver.hahm@inria.fr>
 * @}
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#ifdef MODULE_RTC
#include "rtc.h"

static void _gettime_handler(void)
{
    struct tm now;
    rtc_get_localtime(&now);

    printf("%s", asctime(&now));
}

static void _settime_handler(char **argv)
{
    struct tm now;
    short i1, i2, i3;
    int res;

    do {
        res = sscanf(argv[1], "%hd-%hd-%hd", &i1, &i2, &i3);
        if (res != 3) {
            break;
        }
        now.tm_year = i1 - 1900;
        now.tm_mon = i2 - 1;
        now.tm_mday = i3;

        res = sscanf(argv[2], "%hd:%hd:%hd", &i1, &i2, &i3);
        if (res != 3) {
            break;
        }
        now.tm_hour = i1;
        now.tm_min = i2;
        now.tm_sec = i3;

        rtc_set_localtime(&now);
        puts("OK");
        return;
    } while (0);

    printf("Usage: %s YYYY-MM-DD hh:mm:ss\n", argv[0]);
}

void _date_handler(int argc, char **argv)
{
    if (argc != 3) {
        _gettime_handler();
    }
    else {
        _settime_handler(argv);
    }
}

#endif
