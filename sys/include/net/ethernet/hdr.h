/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

#pragma once

/**
 * @defgroup    net_ethernet_hdr Ethernet header
 * @ingroup     net_ethernet
 * @brief       Ethernet header
 * @{
 *
 * @file
 * @brief       Ethernet header definitions
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */


#include <inttypes.h>

#include "byteorder.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ETHERNET_ADDR_LEN       (6)     /**< Length of an Ethernet address */

#ifndef ETH_ALEN
#define ETH_ALEN ETHERNET_ADDR_LEN      /**< convenient alias for @ref
                                             ETHERNET_ADDR_LEN to comply with
                                             *NIX code */
#endif

/**
 * @brief   Ethernet header
 */
typedef struct __attribute__((packed)) {
    uint8_t dst[ETHERNET_ADDR_LEN];     /**< destination address */
    uint8_t src[ETHERNET_ADDR_LEN];     /**< source address */
    network_uint16_t type;              /**< ether type (see @ref net_ethertype) */
} ethernet_hdr_t;

#ifdef __cplusplus
}
#endif

/**
 * @}
 */
