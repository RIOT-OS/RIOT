/*
 * Copyright (C) 2015 Daniel Krebs
 *               2016 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_gnrc_lwmac
 * @{
 *
 * @file
 * @brief       Header definition LWMAC
 * @internal
 * @author      Daniel Krebs <github@daniel-krebs.net>
 * @author      Shuguo Zhuo  <shuguo.zhuo@inria.fr>
 */

#ifndef NET_GNRC_LWMAC_HDR_H
#define NET_GNRC_LWMAC_HDR_H

#include <stdint.h>
#include <stdbool.h>

#include "net/ieee802154.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   LWMAC WR (wake-up request packet, i.e., preamble packet) frame type
 */
#define GNRC_LWMAC_FRAMETYPE_WR             (0x01U)

/**
 * @brief   LWMAC WA (wake-up answer packet, i.e., preamble-ACK packet) frame type
 */
#define GNRC_LWMAC_FRAMETYPE_WA             (0x02U)

/**
 * @brief   LWMAC data frame type
 */
#define GNRC_LWMAC_FRAMETYPE_DATA           (0x03U)

/**
 * @brief   LWMAC data frame type with pending data transmission request
 */
#define GNRC_LWMAC_FRAMETYPE_DATA_PENDING   (0x04U)

/**
 * @brief   LWMAC broadcast frame type
 */
#define GNRC_LWMAC_FRAMETYPE_BROADCAST      (0x05U)

/**
 * @brief   LWMAC internal L2 address structure
 */
typedef struct {
    uint8_t addr[IEEE802154_LONG_ADDRESS_LEN];  /**< address of node */
    uint8_t len;                                /**< address */
} gnrc_lwmac_l2_addr_t;

/**
 * @brief Static initializer for l2_addr_t.
 */
#define GNRC_LWMAC_L2_ADDR_INITIAL      { { 0 }, 0 }

/**
 * @brief   LWMAC header
 */
typedef struct {
    uint8_t type; /**< type of frame */
} gnrc_lwmac_hdr_t;

/**
 * @brief   LWMAC WR (wake-up request packet, i.e., preamble packet) frame
 */
typedef struct __attribute__((packed)) {
    gnrc_lwmac_hdr_t header;        /**< WR packet header type */
    gnrc_lwmac_l2_addr_t dst_addr;  /**< WR is broadcast, so destination address needed */
} gnrc_lwmac_frame_wr_t;

/**
 * @brief   LWMAC WA (wake-up answer packet, i.e., preamble-ACK packet) frame
 */
typedef struct __attribute__((packed)) {
    gnrc_lwmac_hdr_t header;        /**< WA packet header type */
    gnrc_lwmac_l2_addr_t dst_addr;  /**< WA is broadcast, so destination address needed */
    uint32_t current_phase;         /**< Node's current phase value */
} gnrc_lwmac_frame_wa_t;

/**
 * @brief   LWMAC broadcast data frame
 */
typedef struct __attribute__((packed)) {
    gnrc_lwmac_hdr_t header;    /**< Broadcast packet header type */
    uint8_t seq_nr;             /**< Broadcast sequence */
} gnrc_lwmac_frame_broadcast_t;

/**
 * @brief   LWMAC unicast data frame
 */
typedef struct __attribute__((packed)) {
    gnrc_lwmac_hdr_t header; /**< Data packet header type */
} gnrc_lwmac_frame_data_t;

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* NET_GNRC_LWMAC_HDR_H */
