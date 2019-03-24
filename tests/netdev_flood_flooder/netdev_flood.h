/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief   Common definitions for the netdev flood tests
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef NETDEV_FLOOD_H
#define NETDEV_FLOOD_H

#include "timex.h"

#ifdef __cplusplus
extern "C" {
#endif


#define NETDEV_FLOOD_PKT_SIZE  (48U) /**< size of NETDEV_FLOOD_PAYLOAD + l2 hdr */

/**
 * @brief   The payload for the packets that flood the target
 */
#define NETDEV_FLOOD_PAYLOAD   { 0x53, 0x6f, 0x20, 0x73, 0x6f, 0x72, 0x72, 0x79, \
                                 0x20, 0x66, 0x6f, 0x72, 0x20, 0x66, 0x6c, 0x6f, \
                                 0x6f, 0x64, 0x69, 0x6e, 0x67, 0x20, 0x79, 0x6f, \
                                 0x75, 0x72, 0x20, 0x6e, 0x65, 0x74, 0x77, 0x6f, \
                                 0x72, 0x6b, 0x21, 0x20, 0x3e, 0x2e, 0x3c }

/**
 * @brief   The address of the flooding source
 */
#define NETDEV_FLOOD_SOURCE    { 0x12, 0x0e, 0x2e, 0x0a, 0xf0, 0xc4, 0xb1, 0xe7 }

/**
 * @brief   The address of the flooding target
 */
#define NETDEV_FLOOD_TARGET    { 0x50, 0x0e, 0x61, 0xf2, 0xde, 0x44, 0x8f, 0xb4 }

/**
 * @brief   Flood interval in micro seconds
 */
#define NETDEV_FLOOD_INTERVAL  (5 * US_PER_MS)

#ifdef __cplusplus
}
#endif

#endif /* NETDEV_FLOOD_H */
/** @} */
