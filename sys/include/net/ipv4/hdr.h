/*
 * SPDX-FileCopyrightText: 2015 José Ignacio Alamos <jialamos@uc.cl>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    net_ipv4_hdr    IPv4 header
 * @ingroup     net_ipv4
 * @brief       IPv4 header types and helper functions
 * @{
 *
 * @file
 * @brief   IPv4 header type and helper function definitions
 *
 * @author  José Ignacio Alamos <jialamos@uc.cl>
 */

#include "byteorder.h"
#include "net/ipv4/addr.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief Data type to represent an IPv4 packet header.
 *
 * @details The structure of the header is as follows:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.unparsed}
 *  0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |Version|  IHL  |Type of Service|          Total Length         |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |         Identification        |Flags|      Fragment Offset    |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |  Time to Live |    Protocol   |         Header Checksum       |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                       Source Address                          |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                    Destination Address                        |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                    Options                    |    Padding    |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * @see <a href="https://tools.ietf.org/html/rfc791#section-3.1">
 *              RFC 791, section 3.1
 *      </a>
 */
typedef struct __attribute__((packed)) {
    /**
     * @brief Version and Internet Header Length.
     *
     * @details The version are the 4 most significant bits and the Internet Header Length
     * the 4 next bit (see above).
     *
     * This module provides helper functions to set, get, and check these
     * fields accordingly:
     * * ipv4_hdr_set_version()
     * * ipv4_hdr_get_version()
     * * ipv4_hdr_set_ihl()
     * * ipv4_hdr_get_ihl()
     */
    uint8_t v_ih;
    uint8_t ts;             /**< type of service of packet*/
    network_uint16_t tl;    /**< total length of the datagram */
    network_uint16_t id;    /**< identification value of packet */
    /**
     * @brief version control flags and Fragment Offset.
     *
     * @details The flags are the 3 most significant bits, and the remaining 13 bits are the fragment offset
     *
     * This module provides helper functions to set, get, and check these
     * fields accordingly:
     * * ipv4_hdr_set_flags()
     * * ipv4_hdr_get_flags()
     * * ipv4_hdr_set_fo()
     * * ipv4_hdr_get_fo()
     */
    network_uint16_t fl_fo;
    uint8_t ttl;            /**< time to live for this packet */
    uint8_t protocol;       /**< protocol of this packet */
    network_uint16_t csum;  /**< checksum of this packet */
    ipv4_addr_t src;        /**< source address of this packet */
    ipv4_addr_t dst;        /**< destination address of this packet */
} ipv4_hdr_t;

/**
 * @brief   Sets the version field of @p hdr to 6
 *
 * @param[out] hdr  Pointer to an IPv4 header.
 */
static inline void ipv4_hdr_set_version(ipv4_hdr_t *hdr)
{
    hdr->v_ih &= 0x0f;
    hdr->v_ih |= 0x40;
}

/**
 * @brief   Gets the value of the version field of @p hdr
 *
 * @param[in] hdr   Pointer to an IPv4 header.
 *
 * @return  Value of the version field of @p hdr.
 */
static inline uint8_t ipv4_hdr_get_version(ipv4_hdr_t *hdr)
{
    return ((hdr->v_ih) >> 4);
}

/**
 * @brief   Sets the Internet Header Length field of @p hdr
 *
 * @param[out] hdr  Pointer to an IPv4 header.
 * @param[in] ihl  Size in bytes of the Internet Header Length (including padding)
 */
static inline void ipv4_hdr_set_ihl(ipv4_hdr_t *hdr, uint16_t ihl)
{
    hdr->v_ih &= 0xf0;
    hdr->v_ih |= 0x0f & (ihl >> 5);
}

/**
 * brief Gets the value of the Internet Header Length field of @p hdr
 *
 * @param[in] hdr   Pointer to an IPv4 header.
 *
 * @return Size in bytes of the Internet Header Length field of @p hdr
 */
static inline uint16_t ipv4_hdr_get_ihl(ipv4_hdr_t *hdr)
{
    return (hdr->v_ih & 0x0f) << 5;
}

/**
 * @brief   Sets the Version Control Flags field of @p hdr
 *
 * @param[out] hdr  Pointer to an IPv4 header.
 * @param[in] flags  The new value of flags
 */
static inline void ipv4_hdr_set_flags(ipv4_hdr_t *hdr, uint8_t flags)
{
    hdr->fl_fo.u8[0] &= 0x1f;
    hdr->fl_fo.u8[0] |= (0xe0 & (flags << 5));
}

/**
 * brief Gets the value of the Version Control Flags field of @p hdr
 *
 * @param[in] hdr   Pointer to an IPv4 header.
 *
 * @return Value of the Version Control field of @p hdr
 */
static inline uint8_t ipv4_hdr_get_flags(ipv4_hdr_t *hdr)
{
    return (((hdr->fl_fo.u8[0]) >> 5) & 0x07);
}

/**
 * @brief   Sets the Fragment Offset field of @p hdr
 *
 * @param[out] hdr  Pointer to an IPv4 header.
 * @param[in] fo  The new value of fragment offset
 */
static inline void ipv4_hdr_set_fo(ipv4_hdr_t *hdr, uint16_t fo)
{
    hdr->fl_fo.u8[0] &= 0xe0;
    hdr->fl_fo.u8[0] |= (0x1f & (fo >> 8));
    hdr->fl_fo.u8[1] = 0xff & fo;
}

/**
 * brief Gets the value of the Fragment Offset field of @p hdr
 *
 * @param[in] hdr   Pointer to an IPv4 header.
 *
 * @return Value of the Fragment Offset field of @p hdr
 */
static inline uint16_t ipv4_hdr_get_fo(ipv4_hdr_t *hdr)
{
    return (((hdr->fl_fo.u8[0] & 0x1f) << 8) + hdr->fl_fo.u8[1]);
}

#ifdef __cplusplus
}
#endif

/** @} */
