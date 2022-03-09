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
#include "panic.h"
#include "riotboot/slot.h"
#include "riotboot/serial.h"

static int _get_slot(void)
{
    uint32_t version = 0;
    int slot = -1;

    if (!IS_USED(MODULE_RIOTBOOT_SLOT)) {
        return -1;
    }

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

    return slot;
}

static void _boot_default(int slot)
{
    if (!IS_USED(MODULE_RIOTBOOT_SLOT)) {
        /* boot 'raw' firmware image */
        cpu_jump_to_image(SLOT0_OFFSET);
        return;
    }

    if (slot == -1) {
        slot = _get_slot();
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

NORETURN void core_panic(core_panic_t crash_code, const char *message)
{
    (void)crash_code;
    (void)message;
    while (1) {}
}
