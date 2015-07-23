/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_rpl_srh RPL source routing header extension
 * @ingroup     net_ng_rpl
 * @brief       Implementation of RPL source routing extension headers
 * @see <a href="https://tools.ietf.org/html/rfc6554">
 *          RFC 6554
 *      </a>
 * @{
 *
 * @file
 * @brief       Definititions for RPL source routing extension headers
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef NG_RPL_SRH_H_
#define NG_RPL_SRH_H_

#include "net/ng_ipv6/addr.h"
#include "net/ng_ipv6/ext.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Type for source routing header.
 */
#define NG_RPL_SRH_TYPE     (3U)

/**
 * @brief   The RPL Source routing header.
 *
 * @see <a href="https://tools.ietf.org/html/rfc6554">
 *          RFC 6554
 *      </a>
 *
 * @extends ng_ipv6_ext_rh_t
 */
typedef struct __attribute__((packed)) {
    uint8_t nh;         /**< next header */
    uint8_t len;        /**< length in 8 octets without first octet */
    uint8_t type;       /**< identifier of a particular routing header type */
    uint8_t seg_left;   /**< number of route segments remaining */
} ng_rpl_srh_t;

/**
 * @brief   Extract next hop from the RPL source routing header.
 *
 * @param[in] rh        A RPL source routing header.
 *
 * @return  next hop, on success
 * @return  NULL, if not found.
 */
ng_ipv6_addr_t *ng_rpl_srh_next_hop(ng_rpl_srh_t *rh);

#ifdef __cplusplus
}
#endif

#endif /* NG_RPL_SRH_H_ */
/** @} */
