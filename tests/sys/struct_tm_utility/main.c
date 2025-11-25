/*
 * Copyright (C) 2014  René Kijewski  <rene.kijewski@fu-berlin.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

/**
 * @ingroup  tests
 * @{
 * @file
 * @brief    Test the `struct tm` helpers in "tm.h" of the module "timex".
 * @author   René Kijewski <rene.kijewski@fu-berlin.de>
 * @}
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "shell.h"
#include "tm.h"

static const char MON_NAMES[12][3] = {
    "JAN", "FEB", "MAR", "APR",
    "MAY", "JUN", "JUL", "AUG",
    "SEP", "OCT", "NOV", "DEC",
};
static const char DAY_NAMES[7][3] = {
    "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"
};
static const char BOOL_NAMES[2][3] = { "NO", "YES" };

bool proper_atoi(const char *a, int *i)
{
    char *end;
    *i = strtol(a, &end, 0);
    return (a != end) && (*end == '\0');
}

static int cmd_days_in(int argc, char **argv)
{
    int mon;
    if ((argc != 2) || (proper_atoi(argv[1], &mon) != 1) || (mon < 1) || (mon > 12)) {
        printf("Usage: %s <Month[1..12]>\n", argv[0]);

        return 1;
    }
    else {
        printf("There are %d days in %.3s in common years.\n",
               TM_MON_DAYS[mon - 1], MON_NAMES[mon - 1]);

        return 0;
    }
}

static int cmd_leap_year(int argc, char **argv)
{
    int year;
    if ((argc != 2) || (proper_atoi(argv[1], &year) != 1)) {
        printf("Usage: %s <Year>\n", argv[0]);

        return 1;
    }
    else {
        printf("Was %d a leap year? %.3s.\n",
               year, BOOL_NAMES[tm_is_leap_year(year)]);

        return 0;
    }
}

static int cmd_doomsday(int argc, char **argv)
{
    int year;
    if ((argc != 2) || (proper_atoi(argv[1], &year) != 1)) {
        printf("Usage: %s <Year>\n", argv[0]);

        return 1;
    }
    else {
        printf("What weekday was MAR 0 of %d? %.3s.\n",
               year, DAY_NAMES[tm_doomsday(year) % 7]);

        return 0;
    }
}

static int cmd_day(int argc, char **argv)
{
    int year, mon, day;
    if ((argc != 4) || (proper_atoi(argv[1], &year) != 1)
                    || (proper_atoi(argv[2], &mon) != 1)
                    || (proper_atoi(argv[3], &day) != 1)) {
        printf("Usage: %s <Year> <Month[1..12]> <Day[1..31]>\n", argv[0]);

        return 1;
    }
    else {
        if (!tm_is_valid_date(year, mon - 1, day)) {
            puts("The supplied date is invalid, but no error should occur.");
        }

#ifndef ATMEGA_INCOMPATIBLE_TIME_H
        int wday, yday;
#else
        int8_t wday;
        int16_t yday;
#endif

        tm_get_wyday(year, mon - 1, day, &wday, &yday);
        printf("What weekday was %04d-%02d-%02d? The %d(th) day of the year was a %.3s.\n",
               year, mon, day, yday + 1, DAY_NAMES[wday]);

        return 0;
    }
}

static const shell_command_t shell_commands[] = {
    { "days_in", "Tells you the number of days in a month.", cmd_days_in },
    { "leap_year", "Tells you if a supplied year is a leap year.", cmd_leap_year },
    { "doomsday", "Tells you the wday Doomsday of the supplied year.", cmd_doomsday },
    { "day", "Tells you the day of the supplied date.", cmd_day },
    { NULL, NULL, NULL }
};

int main(void)
{
    puts("`struct tm` utility shell.");

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
