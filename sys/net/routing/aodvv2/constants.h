/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2014 Lotte Steenbrink <lotte.steenbrink@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     aodvv2
 * @{
 *
 * @file
 * @brief       constants for the aodvv2 routing protocol
 *
 * @author      Lotte Steenbrink <lotte.steenbrink@fu-berlin.de>
 */

#ifndef AODVV2_CONSTANTS_H_
#define AODVV2_CONSTANTS_H_

#include "aodvv2/types.h"

#include "common/netaddr.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MANET_PORT  269                 /** RFC5498 */

enum aodvv2_constants {
    AODVV2_MAX_HOPCOUNT = 250,          /**< see AODVv2 draft, section 14.2.*/
    AODVV2_MAX_ROUTING_ENTRIES = 255,   /**< maximum number of entries
                                         *   in the routing table */
    AODVV2_ACTIVE_INTERVAL = 5,         /**< seconds */
    AODVV2_MAX_IDLETIME = 250,          /**< seconds */
    AODVV2_MAX_SEQNUM_LIFETIME = 300,   /**< seconds */
    AODVV2_MAX_UNREACHABLE_NODES = 15,  /**< TODO: choose value (wisely) */
};

/**
 * @brief   TLV type array indices
 */
enum tlv_index
{
    TLV_ORIGSEQNUM,
    TLV_TARGSEQNUM,
    TLV_UNREACHABLE_NODE_SEQNUM,
    TLV_METRIC,
};

/* my multicast address */
extern struct netaddr na_mcast;

/* the interface this protocol operates on */
extern kernel_pid_t aodvv2_if_id;

#ifdef  __cplusplus
}
#endif

#endif /* AODVV2_CONSTANTS_H_ */
