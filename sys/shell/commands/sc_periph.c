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
    do {
        short i1, i2, i3;

        int res = sscanf(argv[0], "%6hd-%6hd-%6hd", &i1, &i2, &i3);
        if (res != 3) {
            break;
        }

        struct tm now;
        now.tm_year = i1 - 1900;
        now.tm_mon = i2 - 1;
        now.tm_mday = i3;

        res = sscanf(argv[1], "%6hd:%6hd:%6hd", &i1, &i2, &i3);
        if (res != 3) {
            break;
        }
        now.tm_hour = i1;
        now.tm_min = i2;
        now.tm_sec = i3;

        rtc_set_time(&now);
        puts("OK");
        return;
    } while (0);

    printf("Usage: %s YYYY-MM-DD hh:mm:ss\n", argv[0]);
}

static void _rtc_usage(void)
{
    puts("usage: rtc <command> [arguments]");
    puts("commands:");
    puts("\tinit\t\tinitialize the interface");
    puts("\tpoweron\t\tpower the interface on");
    puts("\tpoweroff\tpower the interface off");
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
