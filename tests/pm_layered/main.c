/*
 * Copyright (C) 2017 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for pm layered module
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>

#include "shell.h"
#ifdef MODULE_PM_LAYERED
#include "pm_layered.h"
#endif
#include "periph/rtc.h"

#ifdef MODULE_PM_LAYERED
static void _rtc_cb(void *arg)
{
    int level = (int)arg;
    pm_block(level);
}
#endif

static int _pm_unblock(int argc, char **argv)
{
    if (argc < 3) {
        puts("unblock [level] [duration_s]");
        return 1;
    }
#ifdef MODULE_PM_LAYERED
    int level = atoi(argv[1]);
    int duration = atoi(argv[2]);
    printf("Unblocking pm level %d for %ds\n", level, duration);
    struct tm time;
    rtc_get_time(&time);
    time.tm_sec += duration;
    while (time.tm_sec > 60) {
        time.tm_min++;
        time.tm_sec -= 60;
    }
    while (time.tm_min > 60) {
        time.tm_hour++;
        time.tm_min -= 60;
    }
    rtc_set_alarm(&time, _rtc_cb, (void *)level);
    pm_unblock(level);
#else
    (void)argv;
    puts("PM layered not available");
#endif
    return 0;
}

static const shell_command_t commands[] = {
    { "unblock", "pm unblock", _pm_unblock },
    { NULL, NULL, NULL }
};

int main(void)
{
    rtc_init();

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
