/*
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_nimble_autoadv
 * @{
 *
 * @file
 * @brief       Auto advertisement module shell commands
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 * @}
 */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "shell.h"
#include "shell_commands.h"
#include "xfa.h"

#include "nimble_riot.h"
#include "host/ble_hs.h"
#include "host/ble_gap.h"
#include "net/bluetil/ad.h"
#include "net/bluetil/addr.h"
#include "nimble_autoadv.h"

static int _ble_gap_adv_active(void)
{
#if MYNEWT_VAL_BLE_EXT_ADV
    return ble_gap_ext_adv_active(nimble_autoadv_get_adv_instance());
#else
    return ble_gap_adv_active();
#endif
}

static void _print_usage(void)
{
    puts("Usage:");
    puts("\tautoadv start [addr]: start NimBLE auto advertisement");
    puts("\tautoadv stop: stop NimBLE auto advertisement");
    puts("\tautoadv status: print NimBLE auto advertisement status");
}

static int _autoadv_handler(int argc, char **argv)
{
    if (argc < 2) {
        _print_usage();
        return -1;
    }

    if (!strcmp(argv[1], "start")) {
        ble_addr_t *addr_ptr = NULL;
        ble_addr_t addr = { .type = nimble_riot_own_addr_type };
        if (argc == 3) {
            uint8_t addrn[BLE_ADDR_LEN];
            if (bluetil_addr_from_str(addrn, argv[2]) != NULL) {
                /* NimBLE expects address in little endian, so swap */
                bluetil_addr_swapped_cp(addrn, addr.val);
                addr_ptr = &addr;
                puts("Found BLE address: sending directed advertisements");
            }

        }
        nimble_autoadv_start(addr_ptr);
        printf("[autoadv] shell: start advertising on inst=(%d)\n",
               nimble_autoadv_get_adv_instance());
        return 0;
    }

    if (!strcmp(argv[1], "stop")) {
        nimble_autoadv_stop();
        printf("[autoadv] shell: stop advertising on inst=(%d)\n",
               nimble_autoadv_get_adv_instance());
        return 0;
    }

    if (!strcmp(argv[1], "status")) {
        if (_ble_gap_adv_active()) {
            printf("[autoadv] shell: active, inst=(%d)\n",
                   nimble_autoadv_get_adv_instance());
        }
        else {
            puts("[autoadv] shell: inactive\n");
        }
        return 0;
    }

    _print_usage();
    return -1;
}

SHELL_COMMAND(autoadv, "NimBLE autoadv", _autoadv_handler);
