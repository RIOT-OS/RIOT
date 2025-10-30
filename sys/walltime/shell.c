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

#include "fmt.h"
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

static int _cmd_set_time(char **argv)
{
    struct tm now;

    if (scn_time_tm_iso8601_date(&now, argv[0]) < 0) {
        return -1;
    }
    if (scn_time_tm_iso8601_time(&now, argv[1]) < 0) {
        return -2;
    }

    walltime_set(&now);

    return 0;
}

static int cmd_walltime(int argc, char **argv)
{
    if (argc == 1) {
        struct tm now;
        char out[20];

        walltime_get(&now, NULL);
        fmt_time_tm_iso8601(out, &now, ' ');
        print(out, 20);
        print("\n", 1);
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
