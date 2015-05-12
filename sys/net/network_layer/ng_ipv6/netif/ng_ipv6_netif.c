/*
 * Copyright (C) 2014 Martin Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @addtogroup  sys_net_ng_ipv6_netif
 * @{
 *
 * @file
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include <errno.h>
#include <string.h>

#include "kernel_types.h"
#include "mutex.h"
#include "net/ng_ipv6.h"
#include "net/ng_ipv6/addr.h"
#include "net/ng_netif.h"

#include "net/ng_ipv6/netif.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

static ng_ipv6_netif_t ipv6_ifs[NG_NETIF_NUMOF];

#if ENABLE_DEBUG
static char addr_str[NG_IPV6_ADDR_MAX_STR_LEN];
#endif

static int _add_addr_to_entry(ng_ipv6_netif_t *entry, const ng_ipv6_addr_t *addr,
                              uint8_t prefix_len, bool anycast)
{
    for (int i = 0; i < NG_IPV6_NETIF_ADDR_NUMOF; i++) {
        if (ng_ipv6_addr_equal(&(entry->addrs[i].addr), addr)) {
            return 0;
        }

        if (ng_ipv6_addr_is_unspecified(&(entry->addrs[i].addr))) {
            memcpy(&(entry->addrs[i].addr), addr, sizeof(ng_ipv6_addr_t));
            DEBUG("Added %s/%" PRIu8 " to interface %" PRIkernel_pid "\n",
                  ng_ipv6_addr_to_str(&(entry->addrs[i].addr), addr,
                                      sizeof(addr_str)),
                  prefix_len, entry->pid);

            if (anycast || ng_ipv6_addr_is_multicast(addr)) {
                if (ng_ipv6_addr_is_multicast(addr)) {
                    entry->addrs[i].prefix_len = NG_IPV6_ADDR_BIT_LEN;
                }

                entry->addrs[i].flags = NG_IPV6_NETIF_ADDR_FLAGS_NON_UNICAST;

                return 0;
            }
            else {
                entry->addrs[i].prefix_len = prefix_len;

                if (!ng_ipv6_addr_is_link_local(addr)) {
                    /* add also corresponding link-local address */
                    ng_ipv6_addr_t ll_addr;

                    ll_addr.u64[1] = addr->u64[1];
                    ng_ipv6_addr_set_link_local_prefix(&ll_addr);

                    entry->addrs[i].flags = NG_IPV6_NETIF_ADDR_FLAGS_UNICAST;

                    return _add_addr_to_entry(entry, &ll_addr, 64, false);
                }

                return 0;
            }
        }
    }

    return -ENOMEM;
}

static void _reset_addr_from_entry(ng_ipv6_netif_t *entry)
{
    DEBUG("Reset IPv6 addresses on interface %" PRIkernel_pid "\n", entry->pid);
    memset(entry->addrs, 0, sizeof(entry->addrs));
}

void ng_ipv6_netif_init(void)
{
    for (int i = 0; i < NG_NETIF_NUMOF; i++) {
        mutex_init(&(ipv6_ifs[i].mutex));
        mutex_lock(&(ipv6_ifs[i].mutex));
        _reset_addr_from_entry(&ipv6_ifs[i]);
        ipv6_ifs[i].pid = KERNEL_PID_UNDEF;
        mutex_unlock(&(ipv6_ifs[i].mutex));
    }
}

void ng_ipv6_netif_add(kernel_pid_t pid)
{
    for (int i = 0; i < NG_NETIF_NUMOF; i++) {
        if (ipv6_ifs[i].pid == pid) {
            return; /* prevent duplicates */
        }
        else if (ipv6_ifs[i].pid == KERNEL_PID_UNDEF) {
            ng_ipv6_addr_t addr = NG_IPV6_ADDR_ALL_NODES_LINK_LOCAL;
            mutex_lock(&ipv6_ifs[i].mutex);

            DEBUG("Add IPv6 interface %" PRIkernel_pid " (i = %d)\n", pid, i);
            ipv6_ifs[i].pid = pid;
            DEBUG(" * pid = %" PRIkernel_pid "  ", ipv6_ifs[i].pid);
            ipv6_ifs[i].mtu = NG_IPV6_NETIF_DEFAULT_MTU;
            DEBUG("mtu = %d  ", ipv6_ifs[i].mtu);
            ipv6_ifs[i].cur_hl = NG_IPV6_NETIF_DEFAULT_HL;
            DEBUG("cur_hl = %d  ", ipv6_ifs[i].cur_hl);

            _add_addr_to_entry(&ipv6_ifs[i], &addr, NG_IPV6_ADDR_BIT_LEN, 0);

            mutex_unlock(&ipv6_ifs[i].mutex);

            return;
        }
    }

    DEBUG("Could not add %" PRIkernel_pid " to IPv6: No space left.\n", pid);
}

void ng_ipv6_netif_remove(kernel_pid_t pid)
{
    ng_ipv6_netif_t *entry = ng_ipv6_netif_get(pid);

    if (entry == NULL) {
        return;
    }

    mutex_lock(&entry->mutex);

    _reset_addr_from_entry(entry);
    DEBUG("Remove IPv6 interface %" PRIkernel_pid "\n", pid);
    entry->pid = KERNEL_PID_UNDEF;

    mutex_unlock(&entry->mutex);
}

ng_ipv6_netif_t *ng_ipv6_netif_get(kernel_pid_t pid)
{
    for (int i = 0; i < NG_NETIF_NUMOF; i++) {
        if (ipv6_ifs[i].pid == pid) {
            DEBUG("Get IPv6 interface %" PRIkernel_pid " (%p, i = %d)\n", pid,
                  (void *)(&(ipv6_ifs[i])), i);
            return &(ipv6_ifs[i]);
        }
    }

    return NULL;
}

int ng_ipv6_netif_add_addr(kernel_pid_t pid, const ng_ipv6_addr_t *addr,
                           uint8_t prefix_len, bool anycast)
{
    ng_ipv6_netif_t *entry = ng_ipv6_netif_get(pid);
    int res;

    if (entry == NULL) {
        return -ENOENT;
    }

    if ((addr == NULL) || (ng_ipv6_addr_is_unspecified(addr)) ||
        ((prefix_len - 1) > 127)) {    /* prefix_len < 1 || prefix_len > 128 */
        return -EINVAL;
    }

    mutex_lock(&entry->mutex);

    res = _add_addr_to_entry(entry, addr, prefix_len, anycast);

    mutex_unlock(&entry->mutex);

    return res;
}

void ng_ipv6_netif_remove_addr(kernel_pid_t pid, ng_ipv6_addr_t *addr)
{
    ng_ipv6_netif_t *entry = ng_ipv6_netif_get(pid);

    if (entry == NULL) {
        return;
    }

    mutex_lock(&entry->mutex);

    for (int i = 0; i < NG_IPV6_NETIF_ADDR_NUMOF; i++) {
        if (ng_ipv6_addr_equal(&(entry->addrs[i].addr), addr)) {
            DEBUG("Remove %s to interface %" PRIkernel_pid "\n",
                  ng_ipv6_addr_to_str(addr_str, addr, sizeof(addr_str)), pid);
            ng_ipv6_addr_set_unspecified(&(entry->addrs[i].addr));
            entry->addrs[i].flags = 0;

            mutex_unlock(&entry->mutex);
            return;
        }
    }

    mutex_unlock(&entry->mutex);
}

void ng_ipv6_netif_reset_addr(kernel_pid_t pid)
{
    ng_ipv6_netif_t *entry = ng_ipv6_netif_get(pid);

    if (entry == NULL) {
        return;
    }

    mutex_lock(&entry->mutex);

    _reset_addr_from_entry(entry);

    mutex_unlock(&entry->mutex);
}

kernel_pid_t ng_ipv6_netif_find_by_addr(ng_ipv6_addr_t **out, const ng_ipv6_addr_t *addr)
{
    for (int i = 0; i < NG_NETIF_NUMOF; i++) {
        if (out != NULL) {
            *out = ng_ipv6_netif_find_addr(ipv6_ifs[i].pid, addr);

            if (*out != NULL) {
                DEBUG("Found %s on interface %" PRIkernel_pid "\n",
                      ng_ipv6_addr_to_str(addr_str, *out, sizeof(addr_str)),
                      ipv6_ifs[i].pid);
                return ipv6_ifs[i].pid;
            }
        }
        else {
            if (ng_ipv6_netif_find_addr(ipv6_ifs[i].pid, addr) != NULL) {
                DEBUG("Found %s on interface %" PRIkernel_pid "\n",
                      ng_ipv6_addr_to_str(addr_str, *out, sizeof(addr_str)),
                      ipv6_ifs[i].pid);
                return ipv6_ifs[i].pid;
            }
        }
    }

    if (out != NULL) {
        *out = NULL;
    }

    return KERNEL_PID_UNDEF;
}

ng_ipv6_addr_t *ng_ipv6_netif_find_addr(kernel_pid_t pid, const ng_ipv6_addr_t *addr)
{
    ng_ipv6_netif_t *entry = ng_ipv6_netif_get(pid);

    if (entry == NULL) {
        return NULL;
    }

    mutex_lock(&entry->mutex);

    for (int i = 0; i < NG_IPV6_NETIF_ADDR_NUMOF; i++) {
        if (ng_ipv6_addr_equal(&(entry->addrs[i].addr), addr)) {
            mutex_unlock(&entry->mutex);
            DEBUG("Found %s on interface %" PRIkernel_pid "\n",
                  ng_ipv6_addr_to_str(addr_str, addr, sizeof(addr_str)),
                  pid);
            return &(entry->addrs[i].addr);
        }
    }

    mutex_unlock(&entry->mutex);

    return NULL;
}

static uint8_t _find_by_prefix_unsafe(ng_ipv6_addr_t **res, ng_ipv6_netif_t *iface,
                                      const ng_ipv6_addr_t *addr, bool only_unicast)
{
    uint8_t best_match = 0;

    for (int i = 0; i < NG_IPV6_NETIF_ADDR_NUMOF; i++) {
        uint8_t match;

        if ((only_unicast &&
             ng_ipv6_netif_addr_is_non_unicast(&(iface->addrs[i].addr))) ||
            ng_ipv6_addr_is_unspecified(&(iface->addrs[i].addr))) {
            continue;
        }

        match = ng_ipv6_addr_match_prefix(&(iface->addrs[i].addr), addr);

        if (match < iface->addrs[i].prefix_len) {
            /* match but not of same subnet */
            continue;
        }

        if (match > best_match) {
            if (res != NULL) {
                *res = &(iface->addrs[i].addr);
            }

            best_match = match;
        }
    }

#if ENABLE_DEBUG

    if (*res != NULL) {
        DEBUG("Found %s on interface %" PRIkernel_pid " matching ",
              ng_ipv6_addr_to_str(addr_str, *res, sizeof(addr_str)),
              iface->pid);
        DEBUG("%s by %" PRIu8 " bits (used as source address = %s)\n",
              ng_ipv6_addr_to_str(addr_str, addr, sizeof(addr_str)),
              best_match,
              (only_unicast) ? "true" : "false");
    }
    else {
        DEBUG("Did not found any address on interface %" PRIkernel_pid
              " matching %s (used as source address = %s)\n",
              iface->pid,
              ng_ipv6_addr_to_str(addr_str, addr, sizeof(addr_str)),
              (only_unicast) ? "true" : "false");
    }

#endif
    return best_match;
}

kernel_pid_t ng_ipv6_netif_find_by_prefix(ng_ipv6_addr_t **out, const ng_ipv6_addr_t *prefix)
{
    uint8_t best_match = 0;
    ng_ipv6_addr_t *tmp_res = NULL;
    kernel_pid_t res = KERNEL_PID_UNDEF;

    for (int i = 0; i < NG_NETIF_NUMOF; i++) {
        uint8_t match;

        mutex_lock(&(ipv6_ifs[i].mutex));

        match = _find_by_prefix_unsafe(&tmp_res, ipv6_ifs + i, prefix, false);

        if (match > best_match) {
            if (out != NULL) {
                *out = tmp_res;
            }

            res = ipv6_ifs[i].pid;
            best_match = match;
        }

        mutex_unlock(&(ipv6_ifs[i].mutex));
    }

#if ENABLE_DEBUG

    if (res != KERNEL_PID_UNDEF) {
        DEBUG("Found %s on interface %" PRIkernel_pid " globally matching ",
              ng_ipv6_addr_to_str(addr_str, *out, sizeof(addr_str)),
              res);
        DEBUG("%s by %" PRIu8 " bits\n",
              ng_ipv6_addr_to_str(addr_str, prefix, sizeof(addr_str)),
              best_match);
    }
    else {
        DEBUG("Did not found any address globally matching %s\n",
              ng_ipv6_addr_to_str(addr_str, prefix, sizeof(addr_str)));
    }

#endif

    return res;
}

static ng_ipv6_addr_t *_match_prefix(kernel_pid_t pid, const ng_ipv6_addr_t *addr,
                                     bool only_unicast)
{
    ng_ipv6_addr_t *res = NULL;
    ng_ipv6_netif_t *iface = ng_ipv6_netif_get(pid);

    mutex_lock(&(iface->mutex));

    if (_find_by_prefix_unsafe(&res, iface, addr, only_unicast) > 0) {
        mutex_unlock(&(iface->mutex));
        return res;
    }

    mutex_unlock(&(iface->mutex));

    return NULL;
}

ng_ipv6_addr_t *ng_ipv6_netif_match_prefix(kernel_pid_t pid,
        const ng_ipv6_addr_t *prefix)
{
    return _match_prefix(pid, prefix, false);
}

ng_ipv6_addr_t *ng_ipv6_netif_find_best_src_addr(kernel_pid_t pid, const ng_ipv6_addr_t *dest)
{
    return _match_prefix(pid, dest, true);
}

/**
 * @}
 */
