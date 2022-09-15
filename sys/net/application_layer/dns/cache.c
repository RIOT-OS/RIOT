/*
 * Copyright (C) 2022 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_dns_cache
 * @{
 * @file
 * @brief   DNS cache implementation
 * @author  Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @}
 */

#include "bitfield.h"
#include "checksum/fletcher32.h"
#include "mutex.h"
#include "net/af.h"
#include "net/dns/cache.h"
#include "net/ipv4/addr.h"
#include "net/ipv6/addr.h"
#include "time_units.h"
#include "ztimer.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static struct dns_cache_entry {
    uint32_t hash;
    uint32_t expires;
    union {
#if IS_ACTIVE(CONFIG_DNS_CACHE_A)
        ipv4_addr_t v4;
#endif
#if IS_ACTIVE(CONFIG_DNS_CACHE_AAAA)
        ipv6_addr_t v6;
#endif
    } addr;
} cache[CONFIG_DNS_CACHE_SIZE];
static mutex_t cache_mutex = MUTEX_INIT;

#if IS_ACTIVE(CONFIG_DNS_CACHE_A) && IS_ACTIVE(CONFIG_DNS_CACHE_AAAA)
BITFIELD(cache_is_v6, CONFIG_DNS_CACHE_SIZE);

static inline uint8_t _get_len(unsigned idx)
{
    return bf_isset(cache_is_v6, idx) ? 16 : 4;
}
#elif IS_ACTIVE(CONFIG_DNS_CACHE_A)
static inline uint8_t _get_len(unsigned idx)
{
    (void)idx;
    return 4;
}
#elif IS_ACTIVE(CONFIG_DNS_CACHE_AAAA)
static inline uint8_t _get_len(unsigned idx)
{
    (void)idx;
    return 16;
}
#endif

static void _set_len(unsigned idx, uint8_t len)
{
#if IS_ACTIVE(CONFIG_DNS_CACHE_A) && IS_ACTIVE(CONFIG_DNS_CACHE_AAAA)
    if (len == 16) {
        bf_set(cache_is_v6, idx);
    } else {
        bf_unset(cache_is_v6, idx);
    }
#else
    (void)idx;
    (void)len;
#endif
}

static bool _is_empty(unsigned idx)
{
    const uint8_t len = _get_len(idx);
    const uint8_t *addr = (void *)&cache[idx].addr;
    for (unsigned i = 0; i < len; ++i) {
        if (addr[i]) {
            return false;
        }
    }

    return true;
}

static void _set_empty(unsigned idx)
{
    memset(&cache[idx].addr, 0, _get_len(idx));
}

static uint8_t _addr_len(int family)
{
    switch (family) {
#if IS_ACTIVE(CONFIG_DNS_CACHE_A)
    case AF_INET:
        return sizeof(ipv4_addr_t);
#endif
#if IS_ACTIVE(CONFIG_DNS_CACHE_AAAA)
   case AF_INET6:
        return sizeof(ipv6_addr_t);
#endif
    case AF_UNSPEC:
        return 0;
    default:
        return 255;
    }
}

static uint32_t _hash(const void *data, size_t len)
{
    return fletcher32(data, (len + 1) / 2);
}

int dns_cache_query(const char *domain_name, void *addr_out, int family)
{
    int res = 0;
    uint32_t now = ztimer_now(ZTIMER_MSEC) / MS_PER_SEC;
    uint32_t hash = _hash(domain_name, strlen(domain_name));
    uint8_t addr_len = _addr_len(family);

    mutex_lock(&cache_mutex);
    for (unsigned i = 0; i < CONFIG_DNS_CACHE_SIZE; ++i) {
        /* empty slot */
        if (_is_empty(i)) {
            continue;
        }
        /* TTL expired - invalidate slot */
        if (now > cache[i].expires) {
            DEBUG("dns_cache[%u] expired\n", i);
            _set_empty(i);
            continue;
        }
        /* check if hash and length match */
        if (cache[i].hash == hash && (!addr_len || addr_len == _get_len(i))) {
            DEBUG("dns_cache[%u] hit\n", i);
            memcpy(addr_out, &cache[i].addr, _get_len(i));
            res = _get_len(i);
            break;
        }
    }
    if (res == 0) {
        DEBUG("dns_cache miss\n");
    }
    mutex_unlock(&cache_mutex);
    return res;
}

static void _add_entry(uint8_t i, uint32_t hash, const void *addr_out,
                       int addr_len, uint32_t expires)
{
    DEBUG("dns_cache[%u] add cache entry\n", i);
    cache[i].hash = hash;
    cache[i].expires = expires;
    memcpy(&cache[i].addr, addr_out, addr_len);
    _set_len(i, addr_len);
}

void dns_cache_add(const char *domain_name, const void *addr_out,
                        int addr_len, uint32_t ttl)
{
    uint32_t now = ztimer_now(ZTIMER_MSEC) / MS_PER_SEC;
    uint32_t hash = _hash(domain_name, strlen(domain_name));
    uint32_t oldest = ttl;
    int idx = -1;

    assert(addr_len == 4 || addr_len == 16);
    DEBUG("dns_cache: lifetime of %s is %"PRIu32" s\n", domain_name, ttl);

    mutex_lock(&cache_mutex);
    /* iterate even if TTL = 0 just in case we need to expire */
    for (unsigned i = 0; i < CONFIG_DNS_CACHE_SIZE; ++i) {
        if (ttl && (now > cache[i].expires || _is_empty(i))) {
            _add_entry(i, hash, addr_out, addr_len, now + ttl);
            goto exit;
        }
        if (cache[i].hash == hash && _get_len(i) == addr_len) {
            DEBUG("dns_cache[%u] update ttl\n", i);
            if (ttl) {
                cache[i].expires = now + ttl;
            }
            else {
                /* put one second into past so that it is immediately expired */
                cache[i].expires = now - 1;
            }
            goto exit;
        }
        uint32_t _ttl = cache[i].expires - now;
        if (_ttl < oldest) {
            oldest = _ttl;
            idx = i;
        }
    }

    if (ttl && idx >= 0) {
        DEBUG("dns_cache: evict first entry to expire\n");
        _add_entry(idx, hash, addr_out, addr_len, now + ttl);
    }
exit:
    mutex_unlock(&cache_mutex);
}
