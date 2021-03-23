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

#include <assert.h>
#include <string.h>

#include "errno.h"
#include "irq.h"
#include "net/netif.h"
#include "utlist.h"

static list_node_t netif_list;

int netif_register(netif_t *netif)
{
    if (netif == NULL) {
        return -EINVAL;
    }

    unsigned state = irq_disable();
    list_add(&netif_list, &netif->node);
    irq_restore(state);

    return 0;
}

netif_t *netif_iter(netif_t *last)
{
    if (last == NULL) {
        return (netif_t *)netif_list.next;
    }

    return (netif_t *)last->node.next;
}

__attribute__((weak)) int16_t netif_get_id(const netif_t *netif)
{
    list_node_t *node = netif_list.next;
    for (int16_t i = 0; node; i++, node = node->next) {
        if (netif == (netif_t *)node) {
            return i;
        }
    }
    return -1;
}

netif_t *netif_get_by_name(const char *name)
{
    assert(name);
    list_node_t *node = netif_list.next;

    char tmp[CONFIG_NETIF_NAMELENMAX];

    while (node) {
       netif_get_name((netif_t *)node, tmp);
       if (strncmp(name, tmp, CONFIG_NETIF_NAMELENMAX) == 0) {
           return (netif_t *)node;
       }
       node = node->next;
    }

    return NULL;
}

__attribute__((weak)) netif_t *netif_get_by_id(int16_t id)
{
    list_node_t *node = netif_list.next;
    for (int16_t i = 0; node; i++, node = node->next) {
        if (i == id) {
            return (netif_t *)node;
        }
    }
    return NULL;
}

/** @} */
