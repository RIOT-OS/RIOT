/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @defgroup    net_ng_ethernet_hdr Ethernet header
 * @ingroup     net_ng_ethernet
 * @brief       Ethernet header
 * @{
 *
 * @file
 * @brief       Ethernet header definitions
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */


#ifndef NG_ETHERNET_HDR_H_
#define NG_ETHERNET_HDR_H_

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NG_ETHERNET_ADDR_LEN    (6)     /**< Length of an Ethernet address */

/**
 * @brief   Ethernet header
 */
typedef struct __attribute__((packed)) {
    uint8_t dst[NG_ETHERNET_ADDR_LEN];  /**< destination address */
    uint8_t src[NG_ETHERNET_ADDR_LEN];  /**< source address */
    uint16_t type;                      /**< ether type (see @ref net_ng_ethertype) */
} ng_ethernet_hdr_t;

#ifdef __cplusplus
}
#endif

#endif /* NG_ETHERNET_HDR_H_ */
/**
 * @}
 */
