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

#ifdef MODULE_USB_BOARD_RESET
#include "usb_board_reset.h"
#endif

int _reboot_handler(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    pm_reboot();

    return 0;
}

#ifdef MODULE_USB_BOARD_RESET
int _bootloader_handler(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    usb_board_reset_in_bootloader();

    return 0;
}
#endif

int _version_handler(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    puts(RIOT_VERSION);

    return 0;
}
