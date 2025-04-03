/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef NET_ZEP_H
#define NET_ZEP_H
/**
 * @defgroup    net_zep  ZigBee Encapsulation Protocol
 * @ingroup     net
 * @brief       Provides definitions for the ZigBee Encapsulation Protocol (ZEP)
 *
 * @{
 *
 * @file
 * @brief       Definitions for the ZigBee Encapsulation Protocol
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include <stdint.h>

#include "byteorder.h"
#include "net/ntp_packet.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ZEP_PORT_DEFAULT        (17754) /**< default ZEP port */

/**
 * @brief   Type == Data for ZEPv2 header
 */
#define ZEP_V2_TYPE_DATA        (1)

/**
 * @brief   Type == Ack for ZEPv2 header
 */
#define ZEP_V2_TYPE_ACK         (2)

/**
 * @brief   Mask for length field
 */
#define ZEP_LENGTH_MASK         (0x7f)

/**
 * @brief   ZEP header definition
 */
typedef struct __attribute__((packed)) {
    char preamble[2];       /**< Preamble code (must be "EX") */
    uint8_t version;        /**< Protocol Version (must be 1 or 2) */
} zep_hdr_t;

/**
 * @brief   ZEPv1 header definition
 * @extends zep_hdr_t
 */
typedef struct __attribute__((packed)) {
    zep_hdr_t hdr;          /**< common header fields */
    uint8_t chan;           /**< channel ID */
    network_uint16_t dev;   /**< device ID */
    uint8_t lqi_mode;       /**< CRC/LQI Mode (0: append LQI to frame, 1: append FCS) */
    uint8_t lqi_val;        /**< LQI value */
    uint8_t resv[7];        /**< reserved field, must always be 0 */
    uint8_t length;         /**< length of the frame */
} zep_v1_hdr_t;

/**
 * @brief   ZEPv2 header definition (type == Data)
 * @extends zep_hdr_t
 */
typedef struct __attribute__((packed)) {
    zep_hdr_t hdr;          /**< common header fields */
    uint8_t type;           /**< type (must be @ref ZEP_V2_TYPE_DATA) */
    uint8_t chan;           /**< channel ID */
    network_uint16_t dev;   /**< device ID */
    uint8_t lqi_mode;       /**< CRC/LQI Mode */
    uint8_t lqi_val;        /**< LQI value */
    ntp_timestamp_t time;   /**< NTP timestamp */
    network_uint32_t seq;   /**< Sequence number */
    uint8_t resv[10];       /**< reserved field, must always be 0 */
    uint8_t length;         /**< length of the frame */
} zep_v2_data_hdr_t;

/**
 * @brief   ZEPv2 header definition (type == Ack)
 * @extends zep_hdr_t
 */
typedef struct __attribute__((packed)) {
    zep_hdr_t hdr;          /**< common header fields */
    uint8_t type;           /**< type (must be @ref ZEP_V2_TYPE_ACK) */
    network_uint32_t seq;   /**< Sequence number */
} zep_v2_ack_hdr_t;

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* NET_ZEP_H */
