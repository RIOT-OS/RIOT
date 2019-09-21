/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test for wdt Drivers
 *
 *              This test initializes the wdt counter to a preset value
 *              and spins until a wdt reset is triggered
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>

#include "periph/wdt.h"
#include "shell.h"
#include "xtimer.h"

int get_range(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    printf("lower_bound: %d upper_bound: %"PRIu32" \n", NWDT_TIME_LOWER_LIMIT,
           (uint32_t)NWDT_TIME_UPPER_LIMIT);
    return 0;
}

int setup_wdt(int argc, char **argv)
{
    if (argc < 3) {
        printf("usage: %s <min_time[ms]> <max_time[ms]>\n", argv[0]);
        return -1;
    }

    uint32_t min_time = atoi(argv[1]);
    uint32_t max_time = atoi(argv[2]);

    if (max_time > NWDT_TIME_UPPER_LIMIT || max_time < NWDT_TIME_LOWER_LIMIT) {
        puts("invalid times, see \"range\"");
        return -1;
    }

    wdt_setup_reboot(min_time, max_time);
    puts("valid configuration");

    return 0;
}

int start_wdt(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    puts("starting wdt timer");
    wdt_start();
    return 0;
}

int start_loop_wdt(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    printf("start time: %" PRIu32 " us\n", xtimer_now_usec());
    wdt_start();
    while (1) {
        printf("reset time: %" PRIu32 " us\n", xtimer_now_usec());
    }
    return 0;
}

#if WDT_HAS_STOP
int stop_wdt(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    puts("stopping wdt timer");
    wdt_stop();
    return 0;
}
#endif

int kick_wdt(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    puts("delaying wdt timer");
    wdt_kick();
    return 0;
}

static const shell_command_t shell_commands[] = {
    { "range", "Return wdt Timer range", get_range },
    { "setup", "Setup wdt Timer", setup_wdt },
    { "kick", "Delay wdt timer", kick_wdt },
    { "start", "Start wdt timer", start_wdt },
    { "startloop", "Start wdt timer, loop print system time", start_loop_wdt },
#if WDT_HAS_STOP
    { "stop", "Stop wdt timer", stop_wdt },
#endif
    { NULL, NULL, NULL }
};

int main(void)
{
    puts("RIOT wdt test application");
    /* run the shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
