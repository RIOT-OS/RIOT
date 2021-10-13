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

#include "cpu.h"
#include "panic.h"
#include "riotboot/slot.h"

void kernel_init(void)
{
    uint32_t version = 0;
    int slot = -1;

    for (unsigned i = 0; i < riotboot_slot_numof; i++) {
        const riotboot_hdr_t *riot_hdr = riotboot_slot_get_hdr(i);
        if (riotboot_slot_validate(i)) {
            /* skip slot if metadata broken */
            continue;
        }
        if (riot_hdr->start_addr != riotboot_slot_get_image_startaddr(i)) {
            continue;
        }
        if (slot == -1 || riot_hdr->version > version) {
            version = riot_hdr->version;
            slot = i;
        }
    }

    if (slot != -1) {
        riotboot_slot_jump(slot);
    }

    /* serious trouble! nothing to boot */
    while (1) {}
}

NORETURN void core_panic(core_panic_t crash_code, const char *message)
{
    (void)crash_code;
    (void)message;
    while (1) {}
}
