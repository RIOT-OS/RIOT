/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    net_dns DNS defines
 * @ingroup     net
 * @brief       Generic DNS values
 * @{
 *
 * @file
 * @brief   Generic DNS values
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include "modules.h"
#include "net/sock/dns.h"
#include "net/sock/dodtls.h"
#include "net/gcoap/dns.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name DNS defines
 * @{
 */
#define DNS_TYPE_A              (1)
#define DNS_TYPE_AAAA           (28)
#define DNS_CLASS_IN            (1)
/** @} */

/**
 * @name    Field lengths
 * @{
 */
#define RR_TYPE_LENGTH      (2U)
#define RR_CLASS_LENGTH     (2U)
#define RR_TTL_LENGTH       (4U)
#define RR_RDLENGTH_LENGTH  (2U)
/** @} */

/**
 * @brief Get IP address for DNS name
 *
 * This function will synchronously try to resolve a DNS A or AAAA record by contacting
 * the DNS server specified in the global variable @ref sock_dns_server.
 *
 * By supplying AF_INET, AF_INET6 or AF_UNSPEC in @p family requesting of A
 * records (IPv4), AAAA records (IPv6) or both can be selected.
 *
 * This function will return the first DNS record it receives. IF both A and
 * AAAA are requested, AAAA will be preferred.
 *
 * @note @p addr_out needs to provide space for any possible result!
 *       (4byte when family==AF_INET, 16byte otherwise)
 *
 * @param[in]   domain_name     DNS name to resolve into address
 * @param[out]  addr_out        buffer to write result into
 * @param[in]   family          Either AF_INET, AF_INET6 or AF_UNSPEC
 *
 * @return      the size of the resolved address on success
 * @return      < 0 otherwise
 */
static inline int dns_query(const char *domain_name, void *addr_out, int family)
{
    int res = -ENOTSUP;

    if (family == AF_UNSPEC) {
        if (!IS_USED(MODULE_IPV4_ADDR)) {
            family = AF_INET6;
        }
        else if (!IS_USED(MODULE_IPV6_ADDR)) {
            family = AF_INET;
        }
    }

    if (res <= 0 && IS_USED(MODULE_GCOAP_DNS)) {
        res = gcoap_dns_query(domain_name, addr_out, family);
    }
    if (res <= 0 && IS_USED(MODULE_SOCK_DODTLS)) {
        res = sock_dodtls_query(domain_name, addr_out, family);
    }
    if (res <= 0 && (IS_USED(MODULE_SOCK_DNS) || IS_USED(MODULE_SOCK_DNS_MOCK))) {
        res = sock_dns_query(domain_name, addr_out, family);
    }

    return res;
}

#ifdef __cplusplus
}
#endif

/** @} */
