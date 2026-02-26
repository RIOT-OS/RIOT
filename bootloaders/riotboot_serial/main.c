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
 * @brief      Serial UART riot-based bootloader
 *
 * @author     Kaspar Schleiser <kaspar@schleiser.de>
 * @author     Francisco Acosta <francisco.acosta@inria.fr>
 * @author     Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include "cpu.h"
#include "riotboot/bootloader.h"
#include "riotboot/slot.h"
#include "riotboot/serial.h"

static void _boot_default(int slot)
{
    if (!IS_USED(MODULE_RIOTBOOT_SLOT)) {
        /* boot 'raw' firmware image */
        cpu_jump_to_image(SLOT0_OFFSET);
        return;
    }

    if (slot == -1) {
        riotboot_hdr_t riot_hdr;
        slot = riotboot_bootloader_get_slot(&riot_hdr);
    }

    if (slot != -1) {
        riotboot_slot_jump(slot);
    }
}

void kernel_init(void)
{
    int slot = -1;

    if (IS_USED(MODULE_RIOTBOOT_SERIAL)) {
        slot = riotboot_serial_loader();
    }

    irq_disable();
    _boot_default(slot);

    /* serious trouble! nothing to boot */
    while (1) {}
}
