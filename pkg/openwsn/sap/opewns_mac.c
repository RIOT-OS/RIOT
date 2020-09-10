/*
 * Copyright (C) 2019 Hamburg University of Applied Sciences
 *               2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_openwsn
 * @{
 *
 * @file
 *
 * @author      José I. Alamos <jose.alamos@haw-hamburg.de>
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */


#include <stdio.h>

#include "net/mac/sap.h"

#define LOG_LEVEL LOG_INFO
#include "log.h"

mac_t openwsn_mac;

__attribute__((weak)) void mac_mcps_indication(mac_t *mac,
                                                    mcps_indication_t *ind)
{
    (void) mac;
    (void) ind;
}

__attribute__((weak)) void mac_mlme_indication(mac_t *mac,
                                                    mlme_indication_t *ind)
{
    (void) mac;

    if (ind->type == MLME_SYNC) {
        LOG_INFO("[IEE20154E]: synchronized\n");
    }
    else if (ind->type == MLME_SYNC_LOSS) {
        LOG_INFO("[IEE20154E]: desynchronized\n");
    }
    else {
        LOG_INFO("Error: unrecognized MLME indication");
    }
}

__attribute__((weak)) void mac_mcps_confirm(mac_t *mac,
                                                 mcps_confirm_t *confirm)
{
    (void) mac;
    (void) confirm;
}

__attribute__((weak)) void mac_mlme_confirm(mac_t *mac,
                                                 mlme_confirm_t *confirm)
{
    (void) mac;
    if (confirm->type == MLME_SIXTOP_REQ) {
        if(confirm->status == MAC_REQ_STATUS_SUCCESS) {
            LOG_INFO("sixtop request succeeded");
        }
        else {
            LOG_INFO("sixtop request failed");
        }
    }
    else {
        LOG_INFO("Unknown mlme confirm");
    }
}

void mac_init(mac_t *mac, mac_params_t* params)
{
    (void) mac;
    (void) params;
}
