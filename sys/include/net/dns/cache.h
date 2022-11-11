/*
 * Copyright (C) 2022 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_dns_cache   DNS cache
 * @ingroup     net_dns
 *
 * @brief       DNS cache
 *
 * @{
 *
 * @file
 * @brief   DNS cache definitions
 *
 * This implements a simple DNS cache for A and AAAA entries.
 *
 * The cache eviction strategy is based on the remaining time to live
 * of the cache entries, so the first entry to expire will be evicted.
 *
 * This is fine if there are only few cache entries and cache eviction
 * is unlikely.
 * If there is communication to many different hosts, the addition of a
 * least-recently used counter could likely improve the behavior.
 *
 * @author  Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#ifndef NET_DNS_CACHE_H
#define NET_DNS_CACHE_H

#include <stdint.h>

#include "modules.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Maximum number of DNS cache entries
 */
#ifndef CONFIG_DNS_CACHE_SIZE
#define CONFIG_DNS_CACHE_SIZE   4
#endif

/**
 * @brief   Handle to cache A records
 */
#ifndef CONFIG_DNS_CACHE_A
#define CONFIG_DNS_CACHE_A      IS_USED(MODULE_IPV4)
#endif

/**
 * @brief   Handle to cache AAAA records
 */
#ifndef CONFIG_DNS_CACHE_AAAA
#define CONFIG_DNS_CACHE_AAAA   IS_USED(MODULE_IPV6)
#endif

#if IS_USED(MODULE_DNS_CACHE) || DOXYGEN
/**
 * @brief Get IP address for a DNS name from the DNS cache
 *
 * @param[in]   domain_name     DNS name to resolve into address
 * @param[out]  addr_out        buffer to write result into
 * @param[in]   family          Either AF_INET, AF_INET6 or AF_UNSPEC
 *
 * @return      the size of the resolved address on success
 * @return      <= 0 otherwise
 */
int dns_cache_query(const char *domain_name, void *addr_out, int family);

/**
 * @brief Add an IP address for a DNS name to the DNS cache
 *
 * @param[in]   domain_name     DNS name to resolve into address
 * @param[in]   addr            buffer containing the address
 * @param[in]   addr_len        length of the address in bytes
 * @param[in]   ttl             lifetime of the entry in seconds
 */
void dns_cache_add(const char *domain_name, const void *addr, int addr_len, uint32_t ttl);
#else
static inline int dns_cache_query(const char *domain_name, void *addr_out, int family)
{
    (void)domain_name;
    (void)addr_out;
    (void)family;
    return 0;
}

static inline void dns_cache_add(const char *domain_name, const void *addr,
                                 int addr_len, uint32_t ttl)
{
    (void)domain_name;
    (void)addr;
    (void)addr_len;
    (void)ttl;
}
#endif

#ifdef __cplusplus
}
#endif

#endif /* NET_DNS_CACHE_H */
/** @} */
