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

netif_t netif_iter(netif_t last)
{
    gnrc_netif_t *netif;

    if (last == NETIF_INVALID) {
        netif = gnrc_netif_iter(NULL);
    }
    else if ((netif = gnrc_netif_get_by_pid((kernel_pid_t)last)) != NULL) {
        netif = gnrc_netif_iter(netif);
    }
    if (netif != NULL) {
        return netif->pid;
    }
    else {
        return NETIF_INVALID;
    }
}

int netif_get_name(netif_t iface, char *name)
{
    gnrc_netif_t *netif = gnrc_netif_get_by_pid((kernel_pid_t)iface);
    int res = 0;

    if (netif != NULL) {
        res += fmt_str(name, "if");
        res += fmt_u16_dec(&name[res], netif->pid);
        name[res] = '\0';
    }
    return res;
}

netif_t netif_get_by_name(const char *name)
{
    if ((strncmp(name, "if", 2) == 0)) {
        kernel_pid_t _name_pid = (kernel_pid_t)scn_u32_dec(&name[2], 2);
        if (_name_pid > 0) {
            gnrc_netif_t *netif = NULL;

            while ((netif = gnrc_netif_iter(netif)) != NULL) {
                if (netif->pid == _name_pid) {
                    return netif->pid;
                }
            }
        }
    }
    return NETIF_INVALID;
}

int netif_get_opt(netif_t netif, netopt_t opt, uint16_t context,
                  void *value, size_t max_len)
{
    return gnrc_netapi_get(netif, opt, context, value, max_len);
}

int netif_set_opt(netif_t netif, netopt_t opt, uint16_t context,
                  void *value, size_t value_len)
{
    return gnrc_netapi_set(netif, opt, context, value, value_len);
}

/** @} */
