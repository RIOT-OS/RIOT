/**
 * Shell commands for the cc110x driver without a transceiver
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup shell_commands
 * @{
 * @file    sc_cc110x_legacy_csma.c
 * @brief   provides shell commands to configure the cc110x driver
 * @author  Oliver Hahm <oliver.hahm@inria.fr>
 * @author  Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "cc110x_legacy_csma.h"


void _cc110x_get_set_address_handler(int argc, char **argv)
{
    if (argc > 1) {
        int16_t a = atoi(argv[1]);

        printf("[cc110x] Setting address %i ... ", a);
        cc1100_set_address((radio_address_t)a);

        if (cc1100_get_address() == (radio_address_t)a) {
            puts("[OK]");
        }
        else {
            puts("Error!");
        }
    }
    else {
        printf("[cc1100] Got address: %i\n", cc1100_get_address());
    }
}

void _cc110x_get_set_channel_handler(int argc, char **argv)
{
    if (argc > 1) {
        int16_t a = atoi(argv[1]);

        printf("[cc110x] Setting channel %i...", a);
        cc1100_set_channel(a);

        if (cc1100_get_channel() == a) {
            puts("OK");
        }
        else {
            puts("Error!");
        }
    }
    else {
        printf("[cc1100] Got address: %i\n", cc1100_get_channel());
    }
}
