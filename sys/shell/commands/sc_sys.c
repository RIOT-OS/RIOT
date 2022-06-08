/*
 * Copyright (C) 2014  Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
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
 * @brief       Shell commands for system calls
 *
 * @author      Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "periph/pm.h"
#include "shell.h"

#ifdef MODULE_USB_BOARD_RESET
#include "usb_board_reset.h"
#endif

static int _reboot_handler(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    pm_reboot();

    return 0;
}

SHELL_COMMAND(reboot, "Reboot the node", _reboot_handler);

#ifdef MODULE_USB_BOARD_RESET
static int _bootloader_handler(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    usb_board_reset_in_bootloader();

    return 0;
}

SHELL_COMMAND(bootloader, "Reboot to bootloader", _bootloader_handler);
#endif

static int _version_handler(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    puts(RIOT_VERSION);

    return 0;
}

SHELL_COMMAND(version, "Prints current RIOT_VERSION", _version_handler);
