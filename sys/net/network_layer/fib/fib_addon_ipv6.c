/**
 * FIB addon implementation - for ipv6 address types
 *
 * Copyright (C) 2015 Martin Landsmann <Martin.Landsmann@HAW-Hamburg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup fib
 * @{
 * @file    fib_addon_ipv6.c
 * @brief   Functions to manage ipv6 FIB entries
 * @author  Martin Landsmann <Martin.Landsmann@HAW-Hamburg.de>
 * @}
 */

#include <errno.h>
#include "ng_nettype.h"
#include "ng_fib/ng_fib_table.h"
#include "ng_fib/ng_fib_addon_ipv6.h"

#ifndef MODULE_NG_IPV6
#define NG_NETTYPE_IPV6 (28) /**< is AF_INET6 */
#endif

#define ENABLE_DEBUG (0)
#include "debug.h"

int fib_add_entry_ipv6(kernel_pid_t iface_id, ng_ipv6_addr_t *dst,
                       ng_ipv6_addr_t *next_hop,
                       uint32_t lifetime)
{
    mutex_t *mtx_access = fib_get_mutex();
    mutex_lock(mtx_access);
    DEBUG("[fib_add_entry_ipv6]");
    size_t count = 1;
    fib_entry_t *entry[count];

    int ret = fib_find_entry((uint8_t *)dst, sizeof(ng_ipv6_addr_t), &(entry[0]), &count);

    if (ret == 1) {
        /* we must take the according entry and update the values */
        ret = fib_upd_entry(entry[0], (uint8_t *)next_hop, sizeof(ng_ipv6_addr_t), NG_NETTYPE_IPV6, lifetime);
    }
    else {
        ret = fib_create_entry(iface_id, (uint8_t *)dst, sizeof(ng_ipv6_addr_t), NG_NETTYPE_IPV6,
                               (uint8_t *)next_hop, sizeof(ng_ipv6_addr_t), NG_NETTYPE_IPV6, lifetime);
    }

    mutex_unlock(mtx_access);
    return ret;
}

int fib_update_entry_ipv6(ng_ipv6_addr_t *dst,
                          ng_ipv6_addr_t *next_hop,
                          uint32_t lifetime)
{
    mutex_t *mtx_access = fib_get_mutex();
    mutex_lock(mtx_access);
    DEBUG("[fib_update_entry_ipv6]");
    size_t count = 1;
    fib_entry_t *entry[count];
    int ret = -ENOMEM;

    if (fib_find_entry((uint8_t *)dst, sizeof(ng_ipv6_addr_t), &(entry[0]), &count) == 1) {
        DEBUG("[fib_update_entry_ipv6] found entry: %p\n", (void *)(entry[0]));
        /* we must take the according entry and update the values */
        ret = fib_upd_entry(entry[0], (uint8_t *)next_hop, sizeof(ng_ipv6_addr_t), NG_NETTYPE_IPV6, lifetime);
    }
    else {
        /* we have ambigious entries, i.e. count > 1
         * this should never happen
         */
        DEBUG("[fib_update_entry_ipv6] ambigious entries detected!!!");
    }

    mutex_unlock(mtx_access);
    return ret;
}

void fib_remove_entry_ipv6(ng_ipv6_addr_t *dst)
{
    mutex_t *mtx_access = fib_get_mutex();
    mutex_lock(mtx_access);
    DEBUG("[fib_remove_entry_ipv6]");
    size_t count = 1;
    fib_entry_t *entry[count];

    int ret = fib_find_entry((uint8_t *)dst, sizeof(ng_ipv6_addr_t), &(entry[0]), &count);

    if (ret == 1) {
        /* we must take the according entry and update the values */
        fib_remove(entry[0]);
    }
    else {
        /* we have ambigious entries, i.e. count > 1
         * this should never happen
         */
        DEBUG("[fib_update_entry_ipv6] ambigious entries detected!!!");
    }

    mutex_unlock(mtx_access);
}

int fib_get_next_hop_ipv6(kernel_pid_t *iface_id,
                          ng_ipv6_addr_t *next_hop,
                          ng_ipv6_addr_t *dst)
{
    mutex_t *mtx_access = fib_get_mutex();
    mutex_lock(mtx_access);
    DEBUG("[fib_get_next_hop_ipv6]");
    size_t count = 1;
    fib_entry_t *entry[count];

    int ret = fib_find_entry((uint8_t *)dst, sizeof(ng_ipv6_addr_t), &(entry[0]), &count);

    if (!(ret == 0 || ret == 1)) {
        /* notify all RRPs for route discovery if available */
        if (fib_signal_rp((uint8_t *)dst, sizeof(ng_ipv6_addr_t), NG_NETTYPE_IPV6) == 0) {
            count = 1;
            /* now lets see if the RRPs have found a valid next-hop */
            ret = fib_find_entry((uint8_t *)dst, sizeof(ng_ipv6_addr_t), &(entry[0]), &count);
        }
    }

    if (ret == 0 || ret == 1) {
        size_t next_hop_size = sizeof(ng_ipv6_addr_t);
        uint8_t *address_ret = universal_address_get_address(entry[0]->next_hop,
                               (uint8_t *)next_hop, &next_hop_size);

        if (address_ret == NULL) {
            mutex_unlock(mtx_access);
            return -ENOBUFS;
        }
    }
    else {
        mutex_unlock(mtx_access);
        return -EHOSTUNREACH;
    }

    *iface_id = entry[0]->iface_id;
    mutex_unlock(mtx_access);
    return 0;
}
