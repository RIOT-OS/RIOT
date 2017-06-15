/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
 *                    Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     bootloader
 * @{
 *
 * @file
 * @brief       RIOT Bootloader
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 *
 * @}
 */

#include "firmware.h"
#include "cpu.h"
#include "panic.h"

void kernel_init(void)
{
    uint32_t version = 0;
    uint32_t slot = 0;

    /* skip slot 0 (which points to the bootloader) */
    for (unsigned i = 1; i < firmware_num_slots; i++) {
        firmware_metadata_t *slot_metadata = firmware_get_metadata(i);
        if (firmware_validate_metadata_checksum(slot_metadata)) {
            /* skip slot if metadata broken */
            continue;
        }
        if (slot_metadata->start_addr != firmware_get_image_startaddr(i)) {
            continue;
        }
        if (!slot || slot_metadata->version > version) {
            version = slot_metadata->version;
            slot = i;
        }
    }

    if (slot) {
        firmware_jump_to_slot(slot);
    }

    /* serious trouble! */
    while (1) {}
}

NORETURN void core_panic(core_panic_t crash_code, const char *message)
{
    (void)crash_code;
    (void)message;
    while (1) {}
}
