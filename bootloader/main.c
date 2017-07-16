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

#if INTERACTIVE_BOOTLOADER
#include "thread.h"
#include "shell.h"
#include "shell_commands.h"
#endif
#include "fw_slots.h"
#include "cpu_conf.h"

#if INTERACTIVE_BOOTLOADER
static int cmd_lsimg(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    firmware_metadata_t fw_metadata;

    for (uint8_t i = 1; i <= MAX_FW_SLOTS; i++) {
        if (fw_slots_get_int_slot_metadata(i, &fw_metadata) == 0) {
            printf("Metadata slot %d:\n", i);
            fw_slots_print_metadata(&fw_metadata);
        }
        else {
            printf("ERROR: Cannot retrieve metadata from slot %d.\n", i);
        }
    }

    return 0;
}

static int cmd_get_metadata(int argc, char **argv)
{
    uint8_t slot;
    firmware_metadata_t fw_metadata;

    if (argc < 2) {
        printf("usage: %s <slot>\n", argv[0]);
        return -1;
    }

    slot = atoi(argv[1]);

    if (fw_slots_get_int_slot_metadata(slot, &fw_metadata) == 0) {
        printf("Metadata slot %d\n", slot);
        fw_slots_print_metadata(&fw_metadata);
        return 0;
    }
    else {
        printf("ERROR: Cannot retrieve metadata from slot %d.\n", slot);
        return -1;
    }

    return 0;
}

static int cmd_verify(int argc, char **argv)
{
    uint8_t slot;

    if (argc < 2) {
        printf("usage: %s <slot>\n", argv[0]);
        return -1;
    }

    slot = atoi(argv[1]);

    if (fw_slots_verify_int_slot(slot) == 0) {
        printf("Slot %d successfully verified\n", slot);
        return 0;
    }
    else {
        printf("Slot %d not consistent\n", slot);
        return -1;
    }

    return 0;
}

static int cmd_erase_slot(int argc, char**argv)
{
    uint8_t slot;

    if (argc < 2) {
        printf("usage: %s <slot>\n", argv[0]);
        return -1;
    }

    slot = atoi(argv[1]);

    if (fw_slots_erase_int_image(slot) < 0) {
        printf("ERROR: Cannot erase slot %d\n", slot);
        return -1;
    }
    else {
        printf("Slot %d successfully erased\n", slot);
    }

    return 0;
}

static int cmd_jump(int argc, char **argv)
{
    uint32_t address;

    uint8_t slot;

    if (argc < 2) {
        printf("usage: %s <slot>\n", argv[0]);
        return -1;
    }

    slot = atoi(argv[1]);

    if (fw_slots_verify_int_slot(slot) == 0) {
        printf("Slot %d verified!\n", slot);
    }
    else {
        printf("Slot %u inconsistent!\n", slot);
        return -1;
    }

    address = fw_slots_get_slot_address(slot);

    printf("Booting slot %d at 0x%lx...\n", slot, address);

    fw_slots_jump_to_image(address);

    return 0;
}

static const shell_command_t shell_commands[] = {
        { "lsimg", "List the available firmwares on ROM", cmd_lsimg },
        { "get_metadata", "Get metadata from slot", cmd_get_metadata },
        { "verify", "Verify consistency (sha256) of slot", cmd_verify },
        { "erase", "Erase slot *WARNING use with caution*", cmd_erase_slot },
        { "jump", "Jump to specific FW slot (cause reset)", cmd_jump },
        { NULL, NULL, NULL }
};

#else
static int boot_img(void)
{
    uint8_t boot_slot = 0;
    uint32_t appid_slot1 = 0, appid_slot2 = 0;
    firmware_metadata_t metadata;

    puts("[bootlaoder] Cheking for slots metadata...");
    if (fw_slots_get_int_metadata(fw_slots_get_slot_page(1), &metadata) == 0) {
        appid_slot1 = metadata.appid;
        printf("[bootlaoder] Found slot 1 with APPID: 0x%lx \n", appid_slot1);
    }
    else {
        puts("[bootloader] Slot 1 not valid");
    }

    if (fw_slots_get_int_metadata(fw_slots_get_slot_page(2), &metadata) == 0) {
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
            if (fw_slots_verify_int_slot(1) == 0) {
                uint32_t address;
                address = fw_slots_get_slot_address(1);
                fw_slots_jump_to_image(address);
            }
            else {
                puts("[bootlaoder] Slot 1 inconsistent, no image to boot...");
                return -1;
            }
        }
        else {
            puts("[bootloader] Looking for the newest image on ROM, listing...");
            boot_slot = fw_slots_find_newest_int_image();

            if (boot_slot > 0) {
                if (fw_slots_verify_int_slot(boot_slot) == 0) {
                    uint32_t address;
                    printf("[bootloader] newest image on slot %d found and verified! Booting...\n",
                           boot_slot);
                    address = fw_slots_get_slot_address(boot_slot);
                    fw_slots_jump_to_image(address);
                }
                else {
                    printf("Slot %u inconsistent!\n", boot_slot);
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
#endif

int main(void)
{
    (void) puts("Welcome to RIOT bootloader!");
#if INTERACTIVE_BOOTLOADER
    /* run the shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
#else
    boot_img();
#endif
    /* Should never happen */
    return 0;
}
