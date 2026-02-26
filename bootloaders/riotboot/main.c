/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
 *                    Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     bootloaders
 * @{
 *
 * @file
 * @brief      Minimal riot-based bootloader
 *
 * @author     Kaspar Schleiser <kaspar@schleiser.de>
 * @author     Francisco Acosta <francisco.acosta@inria.fr>
 *
 * @}
 */

#include "riotboot/bootloader.h"
#include "riotboot/slot.h"
#include "riotboot/wdt.h"

void kernel_init(void)
{
    riotboot_hdr_t riot_hdr;
    int slot = riotboot_bootloader_get_slot(&riot_hdr);
    if (slot != -1) {
#if MODULE_RIOTBOOT_WDT
        unsigned boot = riotboot_hdr_get_boot_count(&riot_hdr);
        riotboot_wdt_start(CONFIG_RIOTBOOT_WDT_TIMEOUT_MSEC << boot);
#endif
        riotboot_slot_jump(slot);
    }

    /* serious trouble! nothing to boot */
    while (1) {}
}
