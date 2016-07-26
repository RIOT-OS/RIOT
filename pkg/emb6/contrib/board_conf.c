/*
 * Copyright (C) Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include "emb6/netdev2.h"

#include "etimer.h"
#include "board_conf.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

uint8_t board_conf(s_ns_t *ps_nStack)
{
    if (ps_nStack != NULL) {
        ps_nStack->inif = &emb6_netdev2_driver;
        etimer_init();
        return ps_nStack->inif->init(ps_nStack);
    }
    else {
        DEBUG("Network stack pointer is NULL");
        return 0;
    }
}

/** @} */
