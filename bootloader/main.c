/*
 * Copyright (C) 2017 Kaspar Schleiser
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
#include <string.h>
#include <stdio.h>

#include "firmware.h"


static int validate_and_boot(uint8_t slot)
{
    if (firmware_validate_int_slot(slot) != -1) {
        printf("[bootlaoder] slot %d validated! Booting...\n", slot);
        firmware_jump_to_slot(slot);
    }
    else {
        puts("[bootloader] no valid signature!");
        return -1;
    }

    /* Should not happen */
    return -1;
}

static int boot_img(void)
{
    uint8_t boot_slot = 0;
    uint32_t appid_slot1 = 0, appid_slot2 = 0;
    firmware_metadata_t metadata;

    puts("[bootlaoder] Checking for slots metadata...");
    memcpy(&metadata, firmware_get_metadata(1), sizeof(firmware_metadata_t));
    if (firmware_validate_metadata_checksum(&metadata) != -1) {
        appid_slot1 = metadata.appid;
        printf("[bootlaoder] Found slot 1 with APPID: 0x%lx \n", appid_slot1);
    }
    else {
        puts("[bootloader] Slot 1 not valid");
    }

    memcpy(&metadata, firmware_get_metadata(2), sizeof(firmware_metadata_t));
    if (firmware_validate_metadata_checksum(&metadata) != -1) {
        appid_slot2 = metadata.appid;
        printf("[bootlaoder] Found slot 2 with APPID: 0x%lx \n", appid_slot2);
    }
    else {
        puts("[bootloader] Slot 2 not valid");
    }

    if ((appid_slot1 == 0) && (appid_slot2 == 0)) {
        puts("[bootloader] No valid slot found!");
        return -1;
    }
    else {
        if (appid_slot1 != appid_slot2) {
            puts("[bootloader] Warning! application IDs are different!");
            puts("[bootloader] falling back to slot 1");
            if (validate_and_boot(1) == -1) {
                puts("[bootloader] Booting failed!");
            }
            else {
                puts("[bootlaoder] Slot 1 inconsistent, no image to boot...");
                return -1;
            }
        }
        else {
            puts("[bootloader] Looking for the newest image on ROM, listing...");
            boot_slot = firmware_find_newest_int_image();

            if (boot_slot > 0) {
                printf("[bootloader] newest image found on slot %d, checking...\n",
                       boot_slot);
                if (validate_and_boot(boot_slot) == -1) {
                    puts("[bootloader] Booting failed!");
                }
            }
            else {
                (void) puts("No bootable slot found!\n");
                return -1;
            }
        }
    }

    /* Shouldn't happen */
    return 0;
}

int main(void)
{
    return boot_img();

}
