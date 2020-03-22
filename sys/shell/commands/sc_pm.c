/*
 * Copyright (C) 2020 Gunar Schorcht
 * Copyright (C) 2016-2018 Bas Stottelaar <basstottelaar@gmail.com>
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
 * @brief       Shell command implementation for power management
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 * @author      Vincent Dupont <vincent@otakeys.com>
 *
 * @}
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>

#include "periph/pm.h"

#ifdef MODULE_PM_LAYERED
#ifdef MODULE_PERIPH_RTC
#include "periph/rtc.h"
#endif /* MODULE_PERIPH_RTC */
#include "pm_layered.h"
#endif /* MODULE_PM_LAYERED */

#ifdef MODULE_PM_LAYERED
static int _pm_cmd_check_mode(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: %s <power mode>\n", argv[0]);
        return -1;
    }

    return 0;
}

static int _pm_cmd_parse_mode(char *argv)
{
    uint8_t mode = atoi(argv);

    if (mode >= PM_NUM_MODES) {
        printf("Error: power mode not in range 0 - %d.\n", PM_NUM_MODES - 1);
        return -1;
    }

    return mode;
}

#ifdef MODULE_PERIPH_RTC
static int _pm_cmd_check_mode_duration(int argc, char **argv)
{
    if (argc != 3) {
        printf("Usage: %s <power mode> <duration (s)>\n", argv[0]);
        return -1;
    }

    return 0;
}

static int _pm_cmd_parse_duration(char *argv)
{
    int duration = atoi(argv);

    if (duration < 0) {
        puts("Error: duration must be a positive number.");
        return -1;
    }

    return duration;
}

static void _pm_cmd_cb_rtc(void *arg)
{
    int level = (int)arg;

    pm_block(level);
}
#endif /* MODULE_PERIPH_RTC */
#endif /* MODULE_PM_LAYERED */

#ifdef MODULE_PM_LAYERED
static int _pm_cmd_block(int argc, char **argv)
{
    if (_pm_cmd_check_mode(argc, argv) != 0) {
        return 1;
    }

    int mode = _pm_cmd_parse_mode(argv[1]);

    if (mode < 0) {
        return 1;
    }

    printf("Blocking power mode %d.\n", mode);
    fflush(stdout);

    pm_block(mode);

    return 0;
}

static int _pm_cmd_set(int argc, char **argv)
{
    if (_pm_cmd_check_mode(argc, argv) != 0) {
        return 1;
    }

    int mode = _pm_cmd_parse_mode(argv[1]);

    if (mode < 0) {
        return 1;
    }

    printf("CPU will enter power mode %d.\n", mode);
    fflush(stdout);

    pm_set(mode);

    return 0;
}

static int _pm_cmd_unblock(int argc, char **argv)
{
    if (_pm_cmd_check_mode(argc, argv) != 0) {
        return 1;
    }

    int mode = _pm_cmd_parse_mode(argv[1]);

    if (mode < 0) {
        return 1;
    }

    printf("Unblocking power mode %d.\n", mode);
    fflush(stdout);

    pm_unblock(mode);

    return 0;
}

#ifdef MODULE_PERIPH_RTC
static int _pm_cmd_unblock_rtc(int argc, char **argv)
{
    if (_pm_cmd_check_mode_duration(argc, argv) != 0) {
        return 1;
    }

    int mode = _pm_cmd_parse_mode(argv[1]);
    int duration = _pm_cmd_parse_duration(argv[2]);

    if (mode < 0 || duration < 0) {
        return 1;
    }

    printf("Unblocking power mode %d for %d seconds.\n", mode, duration);
    fflush(stdout);

    struct tm time;

    rtc_get_time(&time);
    time.tm_sec += duration;
    mktime(&time);
    rtc_set_alarm(&time, _pm_cmd_cb_rtc, (void *)mode);

    pm_unblock(mode);

    return 0;
}
#endif /* MODULE_PERIPH_RTC */
#endif /* MODULE_PM_LAYERED */

static int _pm_cmd_usage(void)
{
    puts("off\t\tturn off");
    puts("reboot\t\treboot");
#ifdef MODULE_PM_LAYERED
    puts("set\t\tset power mode");
    puts("block\t\tblock power mode");
    puts("unblock\t\tunblock power mode");
#ifdef MODULE_PERIPH_RTC
    puts("unblock_rtc\ttemporary unblock power mode");
#endif /* MODULE_PERIPH_RTC */
#endif /* MODULE_PM_LAYERED */
    return 0;
}

int _pm_cmd_handler(int argc, char **argv)
{
    if (argc < 2) {
        _pm_cmd_usage();
        return 1;
    }
    else if (strncmp(argv[1], "off", 3) == 0) {
        pm_off();
    }
    else if (strncmp(argv[1], "reboot", 6) == 0) {
        pm_reboot();
    }
#ifdef MODULE_PM_LAYERED
    else if ((strncmp(argv[1], "set", 3) == 0) && (argc == 3)) {
        return _pm_cmd_set(argc - 1, argv + 1);
    }
    else if ((strncmp(argv[1], "block", 5) == 0) && (argc == 3)) {
        return _pm_cmd_block(argc - 1, argv + 1);
    }
#ifdef MODULE_PERIPH_RTC
    else if ((strncmp(argv[1], "unblock_rtc", 11) == 0) && (argc == 4)) {
        return _pm_cmd_unblock_rtc(argc - 1, argv + 1);
    }
#endif /* MODULE_PERIPH_RTC */
    else if ((strncmp(argv[1], "unblock", 7) == 0) && (argc == 3)) {
        return _pm_cmd_unblock(argc - 1, argv +1);
    }
#endif /* MODULE_PM_LAYERED */
    else {
        printf("unknown command or missing parameters: %s\n\n", argv[1]);
        _pm_cmd_usage();
        return 1;
    }
    return 0;
}
