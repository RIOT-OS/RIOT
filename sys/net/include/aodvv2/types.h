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
 * @brief       data types for the aodvv2 routing protocol
 *
 * @author      Lotte Steenbrink <lotte.steenbrink@fu-berlin.de>
 */

#ifndef AODVV2_TYPES_H
#define AODVV2_TYPES_H

#include "common/netaddr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   AODVv2 metric types. Extend to include alternate metrics.
 */
typedef enum {
    HOP_COUNT = 3,              /**< see RFC6551*/
} aodvv2_metric_t;

typedef uint16_t aodvv2_seqnum_t;

#define AODVV2_DEFAULT_METRIC_TYPE HOP_COUNT

/**
 * @brief   AODVv2 message types
 */
enum rfc5444_msg_type
{
    RFC5444_MSGTYPE_RREQ = 10,
    RFC5444_MSGTYPE_RREP = 11,
    RFC5444_MSGTYPE_RERR = 12,
};

/**
 * @brief   AODVv2 TLV types
 */
enum rfc5444_tlv_type
{
    RFC5444_MSGTLV_ORIGSEQNUM,
    RFC5444_MSGTLV_TARGSEQNUM,
    RFC5444_MSGTLV_UNREACHABLE_NODE_SEQNUM,
    RFC5444_MSGTLV_METRIC,
};

/**
 * @brief   Data about an OrigNode or TargNode, typically embedded in an
 *          aodvv2_packet_data struct.
 */
struct node_data
{
    struct netaddr addr;                /**< IP address of the node */
    uint8_t metric;                     /**< Metric value */
    aodvv2_seqnum_t seqnum;             /**< Sequence Number */
};

/**
 * @brief   all data contained in a RREQ or RREP.
 */
struct aodvv2_packet_data
{
    uint8_t hoplimit;                   /**< Hop limit */
    struct netaddr sender;              /**< IP address of the neighboring router
                                         *   which sent the RREQ/RREP*/
    aodvv2_metric_t metricType;         /**< Metric type */
    struct node_data origNode;          /**< Data about the originating node */
    struct node_data targNode;          /**< Data about the originating node */
    timex_t timestamp;                  /**< point at which the packet was (roughly)
                                         *   received. Note that this timestamp
                                         *   will be set after the packet has been
                                         *   successfully parsed. */
};

/**
 * @brief   Data about an unreachable node to be embedded in a RERR.
 */
struct unreachable_node
{
    struct netaddr addr;                        /**< IP address */
    aodvv2_seqnum_t seqnum;                     /**< Sequence Number */
};

#ifdef  __cplusplus
}
#endif

#endif /* AODVV2_TYPES_H */
