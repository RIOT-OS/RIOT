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

int _suit_handler(int argc, char **argv)
{
    if (argc != 2) {
        printf("Usage: %s <manifest url>\n", argv[0]);
        return 1;
    }

    suit_coap_trigger((uint8_t *)argv[1], strlen(argv[1]));

    return 0;
}
