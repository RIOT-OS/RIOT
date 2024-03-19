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
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "irq.h"
#include "kernel_defines.h"
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

netif_t *netif_iter(const netif_t *last)
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

netif_t *netif_get_by_name_buffer(const char *name, size_t name_len)
{
    assert(name);

    if (name_len > CONFIG_NETIF_NAMELENMAX) {
        return NULL;
    }

    list_node_t *node = netif_list.next;

    char tmp[CONFIG_NETIF_NAMELENMAX];

    while (node) {
       netif_get_name((netif_t *)node, tmp);
       size_t len = strlen(tmp);
       if ((len == name_len) && (strncmp(name, tmp, name_len) == 0)) {
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

ssize_t netifs_get_ipv6(ipv6_addr_t *dest, size_t numof)
{
    ssize_t result = 0;
    netif_t *netif = NULL;
    while (((netif = netif_iter(netif)) != NULL) && (numof > 0)) {
        ssize_t addrs_numof = netif_get_ipv6(netif, dest, numof);
        if (addrs_numof <= 0) {
            continue;
        }
        result += addrs_numof;
        dest += addrs_numof;
        numof -= addrs_numof;
    }

    return result;
}

#ifndef NETIF_PRINT_IPV6_NUMOF
#define NETIF_PRINT_IPV6_NUMOF 4
#endif

void netif_print_ipv6(netif_t *netif, const char *separator)
{
    ipv6_addr_t addrs[NETIF_PRINT_IPV6_NUMOF];
    ssize_t num = netif_get_ipv6(netif, addrs, ARRAY_SIZE(addrs));
    if (num > 0) {
        ipv6_addrs_print(addrs, num, separator);
    }
}

void netifs_print_ipv6(const char *separator)
{
    netif_t *netif = 0;
    bool first = true;
    while ((netif = netif_iter(netif)) != NULL) {
        ipv6_addr_t addrs[NETIF_PRINT_IPV6_NUMOF];
        ssize_t num = netif_get_ipv6(netif, addrs, ARRAY_SIZE(addrs));
        if (num > 0) {
            if (first) {
                first = false;
            }
            else {
                printf("%s", separator);
            }
            ipv6_addrs_print(addrs, num, separator);
        }
    }
}
/** @} */
