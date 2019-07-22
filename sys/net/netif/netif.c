/*
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Jose I. Alamos <jose.alamos@haw-hamburg.de>
 */

#include <string.h>

#include "errno.h"
#include "net/netif.h"
#include "utlist.h"

static netif_t *netif_list;

int netif_register(netif_t *netif)
{
    if(netif == NULL) {
        return -EINVAL;
    }

    LL_APPEND(netif_list, netif);
    netif->next = NULL;
    return 0;
}

netif_t *netif_iter(netif_t *last)
{
    if (last == NULL) {
        return netif_list;
    }

    return last->next;
}

netif_t *netif_get_by_name(const char *name)
{
   netif_t *netif = netif_list;
   char tmp[NETIF_NAMELENMAX];
   while(netif) {
       netif_get_name(netif, tmp);
       if(strncmp(name, tmp, NETIF_NAMELENMAX) == 0) {
           return netif;
       }
       netif = netif->next;
   }

   return NULL;
}
/** @} */
