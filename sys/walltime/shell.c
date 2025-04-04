/*
 * Copyright (C) 2025 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     sys_walltime
 * @{
 *
 * @file
 * @brief       Wall-Clock time shell commands
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */
#include <stdlib.h>
#include <string.h>

#include "shell.h"
#include "rtc_utils.h"
#include "walltime.h"

static int cmd_uptime(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    uint32_t uptime_full = walltime_uptime(true);
    uint32_t uptime_boot = walltime_uptime(false);

    printf("%"PRIu32" s", uptime_boot);
    if (uptime_boot != uptime_full) {
        printf(" (%"PRIu32" s with hibernation)", uptime_full);
    }
    puts("");

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

static int _cmd_set_time(char **argv)
{
    struct tm now;

    _parse_time(argv, &now);

    walltime_set(&now);

    return 0;
}

static int cmd_walltime(int argc, char **argv)
{
    if (argc == 1) {
        struct tm now;
        walltime_get(&now, NULL);
        _print_time(&now);
    }
    else if (argc == 4 && strcmp(argv[1], "set") == 0) {
        return _cmd_set_time(argv + 2);
    }
    else {
        printf("usage: %s [set <date> <time>]\n", argv[0]);
    }

    return 0;
}

SHELL_COMMAND(uptime, "Seconds since the last reset", cmd_uptime);
SHELL_COMMAND(walltime, "Read and control the system time", cmd_walltime);
