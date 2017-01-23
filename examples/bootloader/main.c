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

#include "thread.h"
#include "shell.h"
#include "shell_commands.h"
#include "fw_slots.h"
#include "cpu_conf.h"

static int cmd_lsimg(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    FW_metadata_t fw_metadata;

    for (uint8_t i = 1; i <= MAX_FW_SLOTS; i++) {
        if (get_int_fw_slot_metadata(i, &fw_metadata) == 0) {
            printf("Metadata slot %d:\n", i);
            print_metadata(&fw_metadata);
        } else {
            printf("ERROR: Cannot retrieve metadata.\n");
        }
    }

    return 0;
}

static int cmd_get_metadata(int argc, char **argv)
{
    uint8_t slot;
    FW_metadata_t fw_metadata;

    if (argc < 2) {
        printf("usage: %s <slot>\n", argv[0]);
        return -1;
    }

    slot = atoi(argv[1]);

    if (get_int_fw_slot_metadata(slot, &fw_metadata) == 0) {
        printf("Metadata slot %d\n", slot);
        print_metadata(&fw_metadata);
        return 0;
    } else {
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

    if (verify_int_fw_slot(slot) == 0) {
        printf("Slot %d successfully verified\n", slot);
        return 0;
    } else {
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

    return erase_int_fw_image(slot);
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

    if (verify_int_fw_slot(slot) == 0) {
        printf("Slot %d verified!\n", slot);
    } else {
        printf("Slot %u inconsistent!\n", slot);
        return -1;
    }

    address = get_slot_fw_address(slot);

    printf("Booting slot %d at 0x%lx...\n", slot, address);

    jump_to_image(address);

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

int main(void)
{
    (void) puts("Welcome to RIOT bootloader!");

    /* run the shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* Should never happen */
    return 0;
}
