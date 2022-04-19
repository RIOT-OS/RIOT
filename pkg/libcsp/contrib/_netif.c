/*
 * Copyright (C) 2022 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * Implements @ref net_netif for @ref net_csp
 *
 * @file
 */

#include <errno.h>
#include <string.h>

#include "csp/csp_iflist.h"
#include "csp/csp_interface.h"
#include "net/netif.h"

int netif_get_name(netif_t *iface, char *name)
{
    csp_iface_t *netif = (csp_iface_t *)iface;

    if (strlen(netif->name) > CONFIG_NETIF_NAMELENMAX) {
        return -ENOBUFS;
    }
    strcpy(name, netif->name);
    return strlen(netif->name);
}

int16_t netif_get_id(const netif_t *netif)
{
    csp_iface_t * iface = csp_iflist_get();
    int16_t id = 0;

    while ((void*)iface != (void*)netif) {
        id++;
        iface = iface->next;
    }
    return id;
}

netif_t *netif_get_by_id(int16_t id)
{
    csp_iface_t * iface = csp_iflist_get();

    while (iface && id) {
        id--;
        iface = iface->next;
    }
    return(netif_t*) iface;
}
/** @} */
