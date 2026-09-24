/*
 * SPDX-FileCopyrightText: 2020 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
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

static void _luid_eui48(netif_type_t type, uint8_t index, eui48_t *addr)
{
    luid_base(addr, sizeof(*addr));

    /* XOR the interface type and index into the least significant bytes
     * so every interface of the same type gets a unique, stable address */
    addr->uint8[4] ^= type;
    addr->uint8[5] ^= index;

    eui48_set_local(addr);
    eui48_clear_group(addr);
}

static void _luid_eui64(netif_type_t type, uint8_t index, eui64_t *addr)
{
    luid_base(addr, sizeof(*addr));

    /* XOR the interface type and index into the least significant bytes
     * so every interface of the same type gets a unique, stable address */
    addr->uint8[6] ^= type;
    addr->uint8[7] ^= index;

    eui64_set_local(addr);
    eui64_clear_group(addr);
}

static void _eui48_get(netif_type_t type, uint8_t index, eui48_t *addr)
{
    unsigned i = EUI48_PROVIDER_NUMOF;
    while (i--) {
#ifdef MODULE_NETIF_REGISTER
        /* using NETIF_ANY causes conflicts if there is another interface
         * of a different type. Require EUI providers to be locked to an
         * interface type for uniqueness.
         */
        assert(eui48_conf[i].type != NETIF_ANY);

        if (eui48_conf[i].type != type) {
            continue;
        }

        if (eui48_conf[i].index != index &&
            eui48_conf[i].index != NETIF_INDEX_ANY) {
            continue;
        }
#endif
        if (eui48_conf[i].provider(index, addr) == 0) {
            return;
        }
    }

    _luid_eui48(type, index, addr);
}

static void _eui64_get(netif_type_t type, uint8_t index, eui64_t *addr)
{
    unsigned i = EUI64_PROVIDER_NUMOF;
    while (i--) {
#ifdef MODULE_NETIF_REGISTER
        /* using NETIF_ANY causes conflicts if there is another interface
         * of a different type. Require EUI providers to be locked to an
         * interface type for uniqueness.
         */
        assert(eui64_conf[i].type != NETIF_ANY);

        if (eui64_conf[i].type != type) {
            continue;
        }

        if (eui64_conf[i].index != index &&
            eui64_conf[i].index != NETIF_INDEX_ANY) {
            continue;
        }
#endif
        if (eui64_conf[i].provider(index, addr) == 0) {
            return;
        }
    }

    _luid_eui64(type, index, addr);
}

void netif_eui48_get(netif_t *netif, eui48_t *addr)
{
#ifdef MODULE_NETIF_REGISTER
    _eui48_get(netif->type, netif->index, addr);
#else
    (void)netif;
    _eui48_get(NETIF_ANY, 0, addr);
#endif
}

void netif_eui64_get(netif_t *netif, eui64_t *addr)
{
#ifdef MODULE_NETIF_REGISTER
    _eui64_get(netif->type, netif->index, addr);
#else
    (void)netif;
    _eui64_get(NETIF_ANY, 0, addr);
#endif
}

void netdev_eui48_get(netdev_t *netdev, eui48_t *addr)
{
#ifdef MODULE_NETIF_REGISTER
    _eui48_get(netdev->type, netdev->index, addr);
#else
    (void)netdev;
    _eui48_get(NETIF_ANY, 0, addr);
#endif
}

void netdev_eui64_get(netdev_t *netdev, eui64_t *addr)
{
#ifdef MODULE_NETIF_REGISTER
    _eui64_get(netdev->type, netdev->index, addr);
#else
    (void)netdev;
    _eui64_get(NETIF_ANY, 0, addr);
#endif
}

/** @} */
