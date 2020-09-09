/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * Implements @ref net_netif for @ref net_gnrc
 *
 * @file
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include <string.h>

#include "fmt.h"
#include "net/gnrc/netapi.h"
#include "net/gnrc/netif/internal.h"

#include "net/netif.h"

int netif_get_name(netif_t *iface, char *name)
{
    gnrc_netif_t *netif = (gnrc_netif_t*) iface;

    int res = 0;
    res += fmt_u16_dec(&name[res], netif->pid);
    name[res] = '\0';
    return res;
}

int16_t netif_get_id(const netif_t *netif)
{
    const gnrc_netif_t *iface = (const gnrc_netif_t*)netif;
    return iface->pid;
}

netif_t *netif_get_by_id(int16_t id)
{
    return &gnrc_netif_get_by_pid((kernel_pid_t)id)->netif;
}

int netif_get_opt(netif_t *netif, netopt_t opt, uint16_t context,
                  void *value, size_t max_len)
{
    gnrc_netif_t *iface = (gnrc_netif_t*) netif;
    return gnrc_netapi_get(iface->pid, opt, context, value, max_len);
}

int netif_set_opt(netif_t *netif, netopt_t opt, uint16_t context,
                  void *value, size_t value_len)
{
    gnrc_netif_t *iface = (gnrc_netif_t*) netif;
    return gnrc_netapi_set(iface->pid, opt, context, value, value_len);
}

/** @} */
