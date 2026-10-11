/*
 * Copyright (C) 2019 Alexandre Abadie <alexandre.abadie@inria.fr>
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
 * @brief       Trigger a firmware update from the shell
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "shell.h"
#include "suit/storage.h"
#include "suit/transport/coap.h"

#ifdef MODULE_SUIT_STORAGE_FLASHWRITE
#include "riotboot/flashwrite.h"
#include "periph/pm.h"
#endif

static void _print_usage(char **argv)
{
    printf("Usage: %s fetch <manifest url>\n", argv[0]);
    printf("       %s seq_no\n", argv[0]);
    if (IS_USED(MODULE_SUIT_STORAGE_FLASHWRITE)) {
        printf("       %s revert\n", argv[0]);
    }
}

static int _suit_handler(int argc, char **argv)
{
    if (argc < 2) {
        _print_usage(argv);
        return 1;
    }

    if (strcmp(argv[1], "fetch") == 0) {
        if (argc > 2) {
            suit_worker_trigger(argv[2], strlen(argv[2]));
        }
        else {
#ifdef SUIT_MANIFEST_RESOURCE
            suit_worker_trigger(SUIT_MANIFEST_RESOURCE, strlen(SUIT_MANIFEST_RESOURCE));
#else
            printf("No manifest URL provided, and SUIT_MANIFEST_RESOURCE not defined.\n");
            return -1;
#endif
        }
    }
    else if (strcmp(argv[1], "seq_no") == 0) {
        uint32_t seq_no = 0;
        suit_storage_get_highest_seq_no(&seq_no);
        printf("seq_no: 0x%08" PRIx32 "\n", seq_no);
    }
#ifdef MODULE_SUIT_STORAGE_FLASHWRITE
    else if (strcmp(argv[1], "revert") == 0) {
        int res = riotboot_flashwrite_invalidate_latest();
        if (res) {
            printf("revert failed: %d\n", res);
        }
        else {
            printf("reverted to previous version, rebooting…\n");
            pm_reboot();
        }
    }
#endif
    else {
        _print_usage(argv);
        return -1;
    }

    return 0;
}

SHELL_COMMAND(suit, "Trigger a SUIT firmware update", _suit_handler);
