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

#include "eui48_provider_params.h"
#include "eui64_provider_params.h"
#include "luid.h"
#include "net/eui_provider.h"

struct netdev_id {
    uint8_t type;
    uint8_t index;
};

void netdev_eui48_get(netdev_t *netdev, eui48_t *addr)
{
    __attribute__((unused))
    static struct netdev_id used[EUI48_PROVIDER_NUMOF];
    unsigned i = EUI48_PROVIDER_NUMOF;

    while (i--) {
#ifdef MODULE_NETDEV_REGISTER
        /* EUI provider already used */
        if (used[i].type  != NETDEV_ANY &&
           (used[i].type  != netdev->type ||
            used[i].index != netdev->index)) {
            continue;
        }

        if (eui48_conf[i].type != netdev->type &&
            eui48_conf[i].type != NETDEV_ANY) {
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
#ifdef MODULE_NETDEV_REGISTER
            used[i].type  = netdev->type;
            used[i].index = netdev->index;
#endif
            return;
        }
    }

    luid_netdev_get_eui48(netdev, addr);
}

void netdev_eui64_get(netdev_t *netdev, eui64_t *addr)
{
    __attribute__((unused))
    static struct netdev_id used[EUI64_PROVIDER_NUMOF];
    unsigned i = EUI64_PROVIDER_NUMOF;

    while (i--) {
#ifdef MODULE_NETDEV_REGISTER
        /* EUI provider already used */
        if (used[i].type  != NETDEV_ANY &&
           (used[i].type  != netdev->type ||
            used[i].index != netdev->index)) {
            continue;
        }

        if (eui64_conf[i].type != netdev->type &&
            eui64_conf[i].type != NETDEV_ANY) {
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
#ifdef MODULE_NETDEV_REGISTER
            used[i].type  = netdev->type;
            used[i].index = netdev->index;
#endif
            return;
        }
    }

    luid_netdev_get_eui64(netdev, addr);
}

/** @} */
