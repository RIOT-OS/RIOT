/*
 * Copyright (C) 2020 Freie Universität Berlin
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
 * @brief       Shell commands to control the NimBLE netif statconn connection
 *              manager
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "net/bluetil/addr.h"
#include "nimble_statconn.h"


static uint8_t _parsephy(const char *phy_str)
{
    if (memcmp(phy_str, "1M", 2) == 0) {
        return NIMBLE_PHY_1M;
    }
#if IS_ACTIVE(MODULE_NIMBLE_PHY_2MBIT)
    else if (memcmp(phy_str, "2M", 2) == 0) {
        return NIMBLE_PHY_2M;
    }
#endif
#if IS_ACTIVE(MODULE_NIMBLE_PHY_CODED)
    else if (memcmp(phy_str, "CODED", 5) == 0) {
        return NIMBLE_PHY_CODED;
    }
#endif
    else {
        return NIMBLE_PHY_INVALID;
    }
}

int _nimble_statconn_handler(int argc, char **argv)
{
    nimble_statconn_cfg_t cfg;

    if ((argc < 3)) {
        printf("usage: %s <addm|adds|rm> <BLE addr> [phy mode]\n"
               "       phy_mode := [1M, 2M, CODED]\n", argv[0]);
        return 0;
    }

    if (argc >= 4) {
        cfg.phy_mode = _parsephy(argv[3]);
        if (cfg.phy_mode == NIMBLE_PHY_INVALID) {
            puts("err: PHY mode not supported");
            return 1;
        }
    }
    else {
        cfg.phy_mode = NIMBLE_STATCONN_PHY_MODE;
    }

    /* parse address */
    uint8_t addr[BLE_ADDR_LEN];
    if (bluetil_addr_from_str(addr, argv[2]) == NULL) {
        puts("err: unable to parse BLE address");
        return 1;
    }

    if (strncmp(argv[1], "addm", 4) == 0) {
        if (nimble_statconn_add_master(addr, &cfg) == 0) {
            puts("success: connecting to peer as slave");
        }
        else {
            puts("err: unable to add peer");
        }
    }
    else if (strncmp(argv[1], "adds", 4) == 0) {
        if (nimble_statconn_add_slave(addr, &cfg) == 0) {
            puts("success: connecting to peer as master");
        }
        else {
            puts("err: unable to add peer");
        }
    }
    else if (strncmp(argv[1], "rm", 2) == 0) {
        if (nimble_statconn_rm(addr) == 0) {
            puts("success: closed connection to peer");
        }
        else {
            puts("err: unable to remove peer");
        }
    }
    else {
        puts("err: unable to parse command");
        return 1;
    }

    return 0;
}
