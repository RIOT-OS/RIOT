/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_rpl_srh RPL source routing header extension
 * @ingroup     net_gnrc_rpl
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
#ifndef NET_GNRC_RPL_SRH_H
#define NET_GNRC_RPL_SRH_H

#include "net/ipv6/hdr.h"
#include "net/ipv6/addr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Type for source routing header.
 */
#define GNRC_RPL_SRH_TYPE   (3U)

/**
 * @brief   The RPL Source routing header.
 *
 * @see <a href="https://tools.ietf.org/html/rfc6554">
 *          RFC 6554
 *      </a>
 *
 * @extends ipv6_ext_rh_t
 */
typedef struct __attribute__((packed)) {
    uint8_t nh;         /**< next header */
    uint8_t len;        /**< length in 8 octets without first octet */
    uint8_t type;       /**< identifier of a particular routing header type */
    uint8_t seg_left;   /**< number of route segments remaining */
    uint8_t compr;      /**< number of prefix octets (comprI and comprE) */
    uint8_t pad_resv;   /**< padding and reserved */
    uint16_t resv;      /**< reserved */
} gnrc_rpl_srh_t;

/**
 * @brief   Process the RPL source routing header.
 *
 * @param[in,out] ipv6  The IPv6 header of the incoming packet.
 * @param[in] rh        A RPL source routing header.
 *
 * @return  EXT_RH_CODE_ERROR
 * @return  EXT_RH_CODE_FORWARD
 * @return  EXT_RH_CODE_OK
 */
int gnrc_rpl_srh_process(ipv6_hdr_t *ipv6, gnrc_rpl_srh_t *rh);

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_RPL_SRH_H */
/** @} */
