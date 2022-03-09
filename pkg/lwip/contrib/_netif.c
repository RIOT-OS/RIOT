/*
 * Copyright (C) 2021 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * Implements @ref net_netif for @ref net_lwip
 *
 * @file
 * @author  Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#include "fmt.h"
#include "lwip/netif.h"
#include "net/netif.h"

int netif_get_name(netif_t *iface, char *name)
{
    struct netif *netif = (struct netif *)iface;

    int res = 2;
    name[0] = netif->name[0];
    name[1] = netif->name[1];
    res += fmt_u16_dec(&name[res], netif->num);
    name[res] = '\0';
    return res;
}

/** @} */
