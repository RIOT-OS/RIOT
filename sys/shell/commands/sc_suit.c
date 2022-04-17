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

#include "suit/transport/coap.h"
#include "suit/storage.h"

static void _print_usage(char **argv)
{
    printf("Usage: %s fetch <manifest url>\n", argv[0]);
    printf("       %s seq_no\n", argv[0]);
}

int _suit_handler(int argc, char **argv)
{
    if (argc < 2) {
        _print_usage(argv);
        return 1;
    }

    if (strcmp(argv[1], "fetch") == 0) {
        suit_coap_trigger((uint8_t *)argv[2], strlen(argv[2]));
    }
    else if (strcmp(argv[1], "seq_no") == 0) {
        uint32_t seq_no = 0;
        suit_storage_get_highest_seq_no(&seq_no);
        printf("seq_no: %" PRIu32 "\n", seq_no);
    }
    else {
        _print_usage(argv);
        return -1;
    }

    return 0;
}
