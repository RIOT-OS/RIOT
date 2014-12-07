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

static int _gettime_handler(void)
{
    struct tm now;
    rtc_get_localtime(&now);

    /* cppcheck: see man 3 asctime: obsoleted by POSIX.1-2008 */
    /* cppcheck-suppress obsoleteFunctionsasctime */
    printf("%s", asctime(&now));

    return 0;
}

static int _settime_handler(char **argv)
{
    do {
        short i1, i2, i3;

        int res = sscanf(argv[1], "%6hd-%6hd-%6hd", &i1, &i2, &i3);
        if (res != 3) {
            break;
        }

        struct tm now;
        now.tm_year = i1 - 1900;
        now.tm_mon = i2 - 1;
        now.tm_mday = i3;

        res = sscanf(argv[2], "%6hd:%6hd:%6hd", &i1, &i2, &i3);
        if (res != 3) {
            break;
        }
        now.tm_hour = i1;
        now.tm_min = i2;
        now.tm_sec = i3;

        rtc_set_localtime(&now);
        puts("OK");
        return 0;
    } while (0);

    printf("Usage: %s YYYY-MM-DD hh:mm:ss\n", argv[0]);
    return 1;
}

int _date_handler(int argc, char **argv)
{
    if (argc != 3) {
        return _gettime_handler();
    }
    else {
        return _settime_handler(argv);
    }
}

#endif
