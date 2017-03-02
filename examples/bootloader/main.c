/*
 * Copyright (C)2016 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Default bootloader application to manage FW slots
 *
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "fw_slots.h"

int main(void)
{
    uint8_t boot_slot = 0;

    (void) puts("Welcome to RIOT bootloader!\n");
    (void) puts("Trying to boot the newest firmware version\n");

    boot_slot = fw_slots_find_newest_int_image();

    if (boot_slot > 0) {
        if (fw_slots_verify_int_slot(boot_slot) == 0) {
            uint32_t address;
            printf("Image on slot %d verified! Booting...\n", boot_slot);
            address = fw_slots_get_slot_address(boot_slot);
            fw_slots_jump_to_image(address);
        } else {
            printf("Slot %u inconsistent!\n", boot_slot);
        }
    } else {
        (void) puts("No bootable slot found!\n");
    }

    /*
     * TODO Add serial bootloader
     */

    /* Should not happen */
    return 0;
}
