/*
 * Copyright (C) 2016 Luminița Lăzărescu <cluminita.lazarescu@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_ntp_packet NTP Packet
 * @ingroup     net
 * @brief       The NTP packet module provides functionality to manipulate the NTP header
 * @{
 *
 * @file
 * @brief       NTP packet definitions
 *
 * @author      Luminița Lăzărescu <cluminita.lazarescu@gmail.com>
 * @author      Martine Lenders <m.lenders@fu-berlin.de>
 */

#ifndef NTP_PACKET_H
#define NTP_PACKET_H

#include <stdint.h>
#include "byteorder.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @{
 * @brief Bit positions and masks for ntp_packet_t::li_vn_mode
 */
/**
 * @brief Leap indicator position
 */
#define NTP_PACKET_LI_POS    (6U)
#define NTP_PACKET_LI_MASK   (0xc0)     /**< leap indicator mask */
#define NTP_PACKET_VN_POS    (3U)       /**< version position */
#define NTP_PACKET_VN_MASK   (0x38)     /**< version mask */
#define NTP_PACKET_MODE_MASK (0x07)     /**< mode mask */
/** @} */

#define NTP_VERSION          (4U)       /**< NTP version */
#define NTP_PORT             (123U)     /**< NTP port number */

/**
 * @brief NTP modes
 */
typedef enum {
    NTP_MODE_RESERVED = 0,  /**< reserved */
    NTP_MODE_SYM_ACTIVE,    /**< symmetric active */
    NTP_MODE_SYM_PASSIVE,   /**< symmetric passive */
    NTP_MODE_CLIENT,        /**< client */
    NTP_MODE_SERVER,        /**< server */
    NTP_MODE_BROADCAST,     /**< broadcast */
    NTP_MODE_PRIV           /**< reserved for private use */
} ntp_mode_t;

/**
 * @brief NTP timestamp
 *
 * @see   [RFC 5905, Section 6](https://tools.ietf.org/html/rfc5905#section-6)
 */
typedef struct __attribute__((packed)) {
    network_uint32_t seconds;           /**< seconds since 1 January 1900 00:00 UTC */
    network_uint32_t fraction;          /**< fraction of seconds in 232 picoseconds */
} ntp_timestamp_t;

/**
 * @brief NTP packet
 *
 * @see   [RFC 5905, Section 7.3](https://tools.ietf.org/html/rfc5905#section-7.3)
 */
typedef struct __attribute__((packed)) {
    uint8_t li_vn_mode;                 /**< leap indicator, version and mode */
    uint8_t stratum;                    /**< stratum */
    uint8_t poll;                       /**< poll in log2 seconds */
    uint8_t precision;                  /**< precision in log2 seconds */
    network_uint32_t root_delay;        /**< root delay in NTP short format */
    network_uint32_t root_dispersion;   /**< root dispersion in NTP short format */
    network_uint32_t reference_id;      /**< reference ID */
    ntp_timestamp_t reference;          /**< reference timestamp */
    ntp_timestamp_t origin;             /**< origin timesptamp */
    ntp_timestamp_t receive;            /**< receive timestamp */
    ntp_timestamp_t transmit;           /**< transmit timestamp */
} ntp_packet_t;

/**
 * @brief Set leap indicator in a NTP packet
 *
 * @param[in] packet    The NTP packet
 * @param[in] li        Leap indicator
 */
static inline void ntp_packet_set_li(ntp_packet_t *packet, uint8_t li)
{
    packet->li_vn_mode &= ~NTP_PACKET_LI_MASK;
    packet->li_vn_mode |= li << NTP_PACKET_LI_POS;
}

/**
 * @brief Set version in a NTP packet
 *
 * @param[in] packet    The NTP packet
 */
static inline void ntp_packet_set_vn(ntp_packet_t *packet)
{
    packet->li_vn_mode &= ~NTP_PACKET_VN_MASK;
    packet->li_vn_mode |= (NTP_VERSION << NTP_PACKET_VN_POS) & NTP_PACKET_VN_MASK;
}

/**
 * @brief Set mode in a NTP packet
 *
 * @param[in] packet    The NTP packet
 * @param[in] mode      Mode
 */
static inline void ntp_packet_set_mode(ntp_packet_t *packet, ntp_mode_t mode)
{
    packet->li_vn_mode &= ~NTP_PACKET_MODE_MASK;
    packet->li_vn_mode |= mode & NTP_PACKET_MODE_MASK;
}

/**
 * @brief Get leap indicator from a NTP packet
 *
 * @param[in] packet    The NTP packet
 *
 * @return The leap indicator of @p packet
 */
static inline uint8_t ntp_packet_get_li(ntp_packet_t *packet)
{
    return (packet->li_vn_mode & NTP_PACKET_LI_MASK) >> NTP_PACKET_LI_POS;
}

/**
 * @brief Get version from a NTP packet
 *
 * @param[in] packet    The NTP packet
 *
 * @return The version of @p packet
 */
static inline uint8_t ntp_packet_get_vn(ntp_packet_t *packet)
{
    return (packet->li_vn_mode & NTP_PACKET_VN_MASK) >> NTP_PACKET_VN_POS;
}

/**
 * @brief Get mode from a NTP packet
 *
 * @param[in] packet   The NTP packet
 *
 * @return The version of @p packet
 */
static inline ntp_mode_t ntp_packet_get_mode(ntp_packet_t *packet)
{
    return (packet->li_vn_mode & NTP_PACKET_MODE_MASK);
}

#ifdef __cplusplus
}
#endif

#endif /* NTP_PACKET_H */
/** @} */
