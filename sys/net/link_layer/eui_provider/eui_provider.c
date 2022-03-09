/*
 * Copyright (C) 2020 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#include "assert.h"
#include "eui48_provider_params.h"
#include "eui64_provider_params.h"
#include "luid.h"
#include "net/eui_provider.h"

void netdev_eui48_get(netdev_t *netdev, eui48_t *addr)
{
    unsigned i = EUI48_PROVIDER_NUMOF;
    while (i--) {
#ifdef MODULE_NETDEV_REGISTER
        /* using NETDEV_ANY causes conflicts if there is another interface
         * of a different type. Require EUI  providers to be locked to an
         * interface type for uniqueness.
         */
        assert(eui48_conf[i].type != NETDEV_ANY);

        if (eui48_conf[i].type != netdev->type) {
            continue;
        }

        if (eui48_conf[i].index != netdev->index &&
            eui48_conf[i].index != NETDEV_INDEX_ANY) {
            continue;
        }
#else
        (void) netdev;
#endif
        if (eui48_conf[i].provider(i, addr) == 0) {
            return;
        }
    }

    luid_netdev_get_eui48(netdev, addr);
}

void netdev_eui64_get(netdev_t *netdev, eui64_t *addr)
{
    unsigned i = EUI64_PROVIDER_NUMOF;
    while (i--) {
#ifdef MODULE_NETDEV_REGISTER
        /* using NETDEV_ANY causes conflicts if there is another interface
         * of a different type. Require EUI  providers to be locked to an
         * interface type for uniqueness.
         */
        assert(eui64_conf[i].type != NETDEV_ANY);

        if (eui64_conf[i].type != netdev->type) {
            continue;
        }

        if (eui64_conf[i].index != netdev->index &&
            eui64_conf[i].index != NETDEV_INDEX_ANY) {
            continue;
        }
#else
        (void) netdev;
#endif
        if (eui64_conf[i].provider(i, addr) == 0) {
            return;
        }
    }

    luid_netdev_get_eui64(netdev, addr);
}

/** @} */
