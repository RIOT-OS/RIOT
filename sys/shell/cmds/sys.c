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

#include "buildinfo/app.h"
#include "buildinfo/riotver.h"
#include "periph/pm.h"
#include "shell.h"

#ifdef MODULE_USB_BOARD_RESET
#include "usb_board_reset.h"
#endif
#ifdef MODULE_RIOTBOOT_SLOT
#include "riotboot/slot.h"
#endif

#ifdef MODULE_PERIPH_PM
static int _reboot_handler(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    pm_reboot();

    return 0;
}

SHELL_COMMAND(reboot, "Reboot the node", _reboot_handler);
#endif /* MODULE_PERIPH_PM */

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

#ifdef MODULE_RIOTBOOT_SLOT
    int slot = riotboot_slot_current();
    if (slot >= 0) {
        const riotboot_hdr_t *hdr = riotboot_slot_get_hdr(slot);
        printf("%s v%"PRIu32", slot %u\n", RIOT_APPLICATION, hdr->version, slot);
    }
#endif
#ifdef CONFIG_RIOT_VERSION_EXTRA
    puts(CONFIG_RIOT_VERSION_EXTRA);
#endif

    return 0;
}

SHELL_COMMAND(version, "Prints current RIOT_VERSION", _version_handler);
