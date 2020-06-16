/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

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
#ifndef NET_DNS_H
#define NET_DNS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   DNS internal structure
 * @see [RFC 6195, section 2]
 *      (https://tools.ietf.org/html/rfc6195#section-2)
 * @see [RFC 1035, section 4.1.1]
 *      (https://tools.ietf.org/html/rfc1035#section-4.1.1)
 */
typedef struct {
    uint16_t id;       /**< Identifier */
    uint16_t flags;    /**< Flags */
    uint16_t qdcount;  /**< Number of entries in the question section */
    uint16_t ancount;  /**< Number of resource records in the answer section */
    uint16_t nscount;  /**< Number of name server resource records */
    uint16_t arcount;  /**< Number of resource records in the additional records
                            section */
    uint8_t payload[]; /**< Payload */
} dns_hdr_t;

/**
 * @name    DNS Record Types
 * @{
 */
#define DNS_TYPE_A          (1)
#define DNS_TYPE_AAAA       (28)
/** @} */

/**
 * @name    DNS Class types
 * @{
 */
#define DNS_CLASS_IN        (1)
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

#ifdef __cplusplus
}
#endif

#endif /* NET_DNS_H */
/** @} */
