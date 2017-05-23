/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include "net/netif.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

netif_params_t *netif_params_get_by_dev(netif_params_t *netif_params,
                                        const void *dev_params)
{
    for (int i = 0; i < NETIF_NUMOF; i++) {
        if (netif_params[i].dev_params == dev_params) {
            return &netif_params[i];
        }
    }
    return NULL;
}

netif_t netif_setup(const netif_params_t *params, unsigned subtype,
                    netdev_t *netdev)
{
    switch (params->type) {
        /* TODO: type specific calls */
        default:
            (void)subtype;
            (void)netdev;
            return NETIF_INVALID;
    }
}

/** @} */
