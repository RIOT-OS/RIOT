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

#ifdef MODULE_GNRC
#include "net/gnrc/netdev.h"
#endif

#include "net/netif.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

/* XXX: rename parameters locally to not conflict with global names in
 * NETIF_NUMOF macro*/
const netif_params_t *netif_params_get_by_dev(const netif_params_t *nparams,
                                              const void *dparams)
{
    for (unsigned i = 0; i < NETIF_NUMOF; i++) {
        if (nparams[i].dev_params == dparams) {
            return &nparams[i];
        }
    }
    DEBUG("netif: found no fitting netif parameters for dev %p\n", dparams);
    return NULL;
}

netif_t netif_setup(const netif_params_t *params, unsigned subtype,
                    netdev_t *netdev, void *priv_data)
{
    switch (params->type) {
        /* TODO: type specific calls */
        default:
            (void)subtype;
            (void)netdev;
            (void)priv_data;
            return NETIF_INVALID;
    }
}

/** @} */
