/*
 * Copyright (C) 2017 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     net_gnrc_gomach
 * @{
 *
 * @file
 * @brief       Header definition of GoMacH
 * @internal
 * @author      Shuguo Zhuo  <shuguo.zhuo@inria.fr>
 */


#include <stdint.h>
#include <stdbool.h>

#include "net/ieee802154.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   GoMacH internal L2 address structure.
 */
typedef struct {
    uint8_t addr[IEEE802154_LONG_ADDRESS_LEN];  /**< Address of node. */
    uint8_t len;                                /**< Address length. */
} gnrc_gomach_l2_addr_t;

/**
 * @brief Static initializer for gnrc_gomach_l2_addr_t.
 */
#define GNRC_GOMACH_L2_ADDR_INIT      { { 0 }, 0 }

/**
 * @brief   GoMacH beacon frame type.
 */
#define GNRC_GOMACH_FRAME_BEACON             (0x01U)

/**
 * @brief   GoMacH data frame type.
 */
#define GNRC_GOMACH_FRAME_DATA               (0x02U)

/**
 * @brief   GoMacH preamble frame type.
 */
#define GNRC_GOMACH_FRAME_PREAMBLE           (0x03U)

/**
 * @brief   GoMacH preamble-ACK frame type.
 */
#define GNRC_GOMACH_FRAME_PREAMBLE_ACK       (0x04U)

/**
 * @brief   GoMacH broadcast frame type.
 */
#define GNRC_GOMACH_FRAME_BROADCAST          (0x05U)

/**
 * @brief   GoMacH announce frame type.
 *
 * This frame type is specifically used to announce the chosen sub-channel
 * sequence of the node to its one-hop neighbors.
 */
#define GNRC_GOMACH_FRAME_ANNOUNCE           (0x06U)

/**
 * @brief   GoMacH frame header.
 */
typedef struct {
    uint8_t type;  /**< Type of GoMacH frame. */
} gnrc_gomach_hdr_t;

/**
 * @brief   GoMacH Beacon frame
 */
typedef struct __attribute__((packed)) {
    gnrc_gomach_hdr_t header;       /**< Beacon frame header type. */
    uint8_t sub_channel_seq;        /**< Sub-channel sequence of this node. */
    uint8_t schedulelist_size;      /**< vTDMA schedule list size. */
} gnrc_gomach_frame_beacon_t;

/**
 * @brief   GoMacH data frame
 */
typedef struct __attribute__((packed)) {
    gnrc_gomach_hdr_t header;       /**< Data frame header type. */
    uint8_t queue_indicator;        /**< Queue-length indicator of this node. */
} gnrc_gomach_frame_data_t;

/**
 * @brief   GoMacH sub-channel announce frame.
 *
 * This frame type is specifically used to announce the chosen sub-channel
 * sequence of the node to its one-hop neighbors.
 */
typedef struct __attribute__((packed)) {
    gnrc_gomach_hdr_t header;       /**< Announce frame header type. */
    uint8_t subchannel_seq;         /**< Sub-channel sequence of this node. */
} gnrc_gomach_frame_announce_t;

/**
 * @brief   GoMacH preamble frame.
 */
typedef struct __attribute__((packed)) {
    gnrc_gomach_hdr_t header;           /**< Preamble frame header type. */
    gnrc_gomach_l2_addr_t dst_addr;     /**< Address of this node. */
} gnrc_gomach_frame_preamble_t;

/**
 * @brief   GoMacH preamble_ack frame.
 */
typedef struct __attribute__((packed)) {
    gnrc_gomach_hdr_t header;           /**< Preamble-ACK frame header type. */
    gnrc_gomach_l2_addr_t dst_addr;     /**< Address of this node. */
    uint32_t phase_in_us;               /**< Current phase of this node. */
} gnrc_gomach_frame_preamble_ack_t;

/**
 * @brief   GoMacH broadcast frame.
 */
typedef struct __attribute__((packed)) {
    gnrc_gomach_hdr_t header;           /**< Broadcast frame header type. */
    uint8_t seq_nr;                     /**< Broadcast sequence of this node. */
} gnrc_gomach_frame_broadcast_t;

#ifdef __cplusplus
}
#endif

/** @} */
