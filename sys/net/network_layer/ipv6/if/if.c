/*
 * Copyright (C) 2014 Martin Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @addtogroup  ipv6_if
 * @{
 *
 * @file        if.c
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include <errno.h>

#include "kernel_macros.h"
#include "ipv6.h"
#include "ipv6/if.h"

#define _CHECK(if_id, err_ret) \
    if (!ipv6_if_initialized(if_id)) { \
        err_ret; \
    };

ipv6_if_t ipv6_ifs[IPV6_IF_NUM];

const ipv6_if_t ipv6_if_loopback = {
    { { IPV6_ADDR_LOOPBACK, IPV6_IF_ADDR_FLAGS_UNICAST } },
    MUTEX_INIT,
    KERNEL_PID_UNDEF,
    IPV6_DEFAULT_MTU
};

static inline void _reset_addr(ipv6_if_addr_t *a)
{
    a->addr.u64[0].u64 = 0;
    a->addr.u64[1].u64 = 0;
    a->flags = IPV6_IF_ADDR_FLAGS_UNICAST;
}

static void _reset_if(ipv6_if_t *ipv6_if)
{
    ipv6_if->mac_pid = KERNEL_PID_UNDEF;
    ipv6_if->mtu = IPV6_DEFAULT_MTU;

    for (int i = 0; i < IPV6_IF_ADDR_NUM; i++) {
        _reset_addr(ipv6_if->addrs + i);
    }
}

void ipv6_if_init(void)
{
    for (int if_id = 0; if_id < IPV6_IF_NUM; if_id++) {
        mutex_init(&(ipv6_ifs[if_id].mutex));
        mutex_lock(&(ipv6_ifs[if_id].mutex));

        _reset_if(&(ipv6_ifs[if_id]));

        mutex_unlock(&(ipv6_ifs[if_id].mutex));
    }
}

int ipv6_if_init_if(kernel_pid_t mac_pid)
{
    if (mac_pid == KERNEL_PID_UNDEF) {
        return -EINVAL;
    }

    for (int if_id = 0; if_id < IPV6_IF_NUM; if_id++) {
        if (ipv6_ifs[if_id].mac_pid == mac_pid) {
            return -EISCONN;
        }

        if (ipv6_ifs[if_id].mac_pid == KERNEL_PID_UNDEF) {
            mutex_lock(&(ipv6_ifs[if_id].mutex));

            ipv6_ifs[if_id].mac_pid = mac_pid;

            mutex_unlock(&(ipv6_ifs[if_id].mutex));

            return if_id;
        }
    }

    return -ENOBUFS;
}

void ipv6_if_reset_if(int if_id)
{
    if (ipv6_if_initialized(if_id)) {
        mutex_lock(&(ipv6_ifs[if_id].mutex));

        _reset_if(&(ipv6_ifs[if_id]));

        mutex_unlock(&(ipv6_ifs[if_id].mutex));
    }
}

int ipv6_if_get_id_by_mac(kernel_pid_t mac_pid)
{
    if (mac_pid == KERNEL_PID_UNDEF) {
        return -ENOENT;
    }

    for (int if_id = 0; if_id < IPV6_IF_NUM; if_id++) {
        mutex_lock(&(ipv6_ifs[if_id].mutex));

        if (ipv6_ifs[if_id].mac_pid == mac_pid) {
            mutex_unlock(&(ipv6_ifs[if_id].mutex));
            return if_id;
        }

        mutex_unlock(&(ipv6_ifs[if_id].mutex));
    }

    return -ENOENT;
}

int ipv6_if_add_addr(int if_id, const ipv6_addr_t *addr, bool anycast)
{
    int res = -ENOBUFS;

    _CHECK(if_id, return -ENOENT);

    if (ipv6_addr_is_unspecified(addr) || ipv6_addr_is_loopback(addr) ||
        (ipv6_addr_is_multicast(addr) && anycast)) {
        return -EINVAL;
    }

    mutex_lock(&(ipv6_ifs[if_id].mutex));

    for (int i = 0; i < IPV6_IF_ADDR_NUM; i++) {
        if (ipv6_addr_is_unspecified(&(ipv6_ifs[if_id].addrs[i].addr))) {
            ipv6_ifs[if_id].addrs[i].addr.u64[0] = addr->u64[0];
            ipv6_ifs[if_id].addrs[i].addr.u64[1] = addr->u64[1];

            if (anycast) {
                ipv6_ifs[if_id].addrs[i].flags |= IPV6_IF_ADDR_FLAGS_ANYCAST;
            }

            if (ipv6_addr_is_multicast(addr)) {
                ipv6_ifs[if_id].addrs[i].flags |= IPV6_IF_ADDR_FLAGS_MULTICAST;
            }

            if (ipv6_addr_is_link_local(addr)) {
                ipv6_ifs[if_id].addrs[i].flags |= IPV6_IF_ADDR_FLAGS_LINK_LOCAL;
            }

            res = 0;

            break;
        }
        else if (ipv6_addr_equal(&(ipv6_ifs[if_id].addrs[i].addr), addr)) {
            res = 0;

            break;
        }
    }

    mutex_unlock(&(ipv6_ifs[if_id].mutex));

    return res;
}

static ipv6_addr_t *_ipv6_if_find_addr_on_if(ipv6_if_t *ipv6_if,
        const ipv6_addr_t *addr)
{
    for (int i = 0; i < IPV6_IF_ADDR_NUM; i++) {
        if (ipv6_addr_equal(addr, &(ipv6_if->addrs[i].addr))) {
            return &(ipv6_if->addrs[i].addr);
        }
    }

    return NULL;
}

int ipv6_if_rem_addr(int if_id, const ipv6_addr_t *addr)
{
    ipv6_addr_t *if_addr;

    _CHECK(if_id, return -ENOENT);

    mutex_lock(&(ipv6_ifs[if_id].mutex));

    if ((if_addr = _ipv6_if_find_addr_on_if(&(ipv6_ifs[if_id]), addr)) != NULL) {
        _reset_addr(container_of(if_addr, ipv6_if_addr_t, addr));
    }

    mutex_unlock(&(ipv6_ifs[if_id].mutex));

    return 0;
}

ipv6_addr_t *ipv6_if_iter_addr(int if_id, const ipv6_addr_t *prev)
{
    ipv6_if_addr_t *res = NULL;

    if (ipv6_if_initialized(if_id)) {
        if (if_id == IPV6_IF_LOOPBACK_ID) {
            if (prev == NULL) {
                res = (ipv6_if_addr_t *) ipv6_if_loopback.addrs;
            }
        }
        else {
            if ((prev >= &(ipv6_ifs[if_id].addrs[0].addr) &&
                 (prev < &(ipv6_ifs[if_id].addrs[IPV6_IF_ADDR_NUM - 1].addr)))) {
                res = (container_of(prev, ipv6_if_addr_t, addr)) + 1;
            }
            else if (prev == NULL) {
                res = &(ipv6_ifs[if_id].addrs[0]);
            }
            else {
                return NULL;
            }

            while (ipv6_addr_is_unspecified(&(res->addr)) &&
                   res < &(ipv6_ifs[if_id].addrs[IPV6_IF_ADDR_NUM - 1])) {
                res++;
            }

            if (ipv6_addr_is_unspecified(&(res->addr))) {
                res = NULL;
            }
        }
    }

    return &(res->addr);
}

ipv6_addr_t *ipv6_if_find_addr_on_if(int if_id, const ipv6_addr_t *addr)
{
    ipv6_addr_t *res;
    _CHECK(if_id, return NULL);

    mutex_lock(&(ipv6_ifs[if_id].mutex));

    if (ipv6_addr_is_unspecified(addr) || ipv6_addr_is_loopback(addr)) {
        return NULL;
    }

    res = _ipv6_if_find_addr_on_if(ipv6_ifs + if_id, addr);

    mutex_unlock(&(ipv6_ifs[if_id].mutex));

    return res;
}

ipv6_if_addr_pair_t *ipv6_if_find_addr(ipv6_if_addr_pair_t *res,
                                       const ipv6_addr_t *addr)
{
    if (ipv6_addr_is_unspecified(addr)) {
        return NULL;
    }

    if (ipv6_addr_is_loopback(addr)) {
        res->if_id = IPV6_IF_LOOPBACK_ID;
        res->addr = (ipv6_addr_t *)&ipv6_if_loopback.addrs[0].addr; /* ipv6_if_loopback is const */

        return res;
    }

    for (int if_id = 0; if_id < IPV6_IF_NUM; if_id++) {
        mutex_lock(&(ipv6_ifs[if_id].mutex));

        res->addr = _ipv6_if_find_addr_on_if(&(ipv6_ifs[if_id]), addr);

        if (res->addr != NULL) {
            res->if_id = if_id;
            mutex_unlock(&(ipv6_ifs[if_id].mutex));
            return res;
        }

        mutex_unlock(&(ipv6_ifs[if_id].mutex));
    }

    return NULL;
}

static uint8_t _find_best_prefix_on_if(ipv6_addr_t **res, ipv6_if_t *ipv6_if,
                                       const ipv6_addr_t *addr, bool with_multicast)
{
    uint8_t best_match = 0;

    for (int i = 0; i < IPV6_IF_ADDR_NUM; i++) {
        uint8_t match;

        if (ipv6_addr_is_unspecified(&(ipv6_if->addrs[i].addr)) ||
            ((!with_multicast) && ipv6_if_addr_is_multicast(&(ipv6_if->addrs[i].addr)))) {
            continue;
        }

        match = ipv6_addr_match_prefix(&(ipv6_if->addrs[i].addr), addr);

        if (match > best_match) {
            *res = &(ipv6_if->addrs[i].addr);
            best_match = match;
        }
    }

    return best_match;
}

static ipv6_addr_t *_find_best_prefix_on_if2(int if_id, const ipv6_addr_t *addr,
        bool with_multicast)
{
    ipv6_addr_t *res = NULL;
    _CHECK(if_id, return NULL);

    mutex_lock(&(ipv6_ifs[if_id].mutex));

    if (_find_best_prefix_on_if(&res, ipv6_ifs + if_id, addr,
                                with_multicast) > 0) {
        mutex_unlock(&(ipv6_ifs[if_id].mutex));
        return res;
    }

    mutex_unlock(&(ipv6_ifs[if_id].mutex));

    return NULL;
}

static ipv6_if_addr_pair_t *_find_best_prefix(ipv6_if_addr_pair_t *res,
        const ipv6_addr_t *addr,
        bool with_multicast)
{
    uint8_t best_match = 0;
    ipv6_addr_t *tmp_res = NULL;

    for (int if_id = 0; if_id < IPV6_IF_NUM; if_id++) {
        uint8_t match;

        mutex_lock(&(ipv6_ifs[if_id].mutex));

        match = _find_best_prefix_on_if(&tmp_res, &(ipv6_ifs[if_id]), addr, with_multicast);

        if (match > best_match) {
            res->addr = tmp_res;
            res->if_id = if_id;
            best_match = match;
        }

        mutex_unlock(&(ipv6_ifs[if_id].mutex));
    }

    if (best_match > 0) {
        return res;
    }
    else {
        return NULL;
    }
}

ipv6_addr_t *ipv6_if_find_prefix_match_on_if(int if_id, const ipv6_addr_t *addr)
{
    return _find_best_prefix_on_if2(if_id, addr, true);
}

ipv6_if_addr_pair_t *ipv6_if_find_prefix_match(ipv6_if_addr_pair_t *res,
        const ipv6_addr_t *addr)
{
    return _find_best_prefix(res, addr, true);
}

ipv6_addr_t *ipv6_if_get_best_src_addr_on_if(int if_id, const ipv6_addr_t *addr)
{
    return _find_best_prefix_on_if2(if_id, addr, false);
}

ipv6_if_addr_pair_t *ipv6_if_get_best_src_addr(ipv6_if_addr_pair_t *res,
        const ipv6_addr_t *addr)
{
    return _find_best_prefix(res, addr, false);
}

/**
 * @}
 */
