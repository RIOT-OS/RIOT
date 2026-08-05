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
 * @author  Bas Stottelaar <basstottelaar@gmail.com>
 */

#include <assert.h>

#include "byteorder.h"
#include "net/inet_csum.h"
#include "net/ipv4/addr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    IPv4 Header Flags
 * @see     [RFC 791, section 3.1](https://tools.ietf.org/html/rfc791#section-3.1)
 * @{
 */
#define IPV4_HDR_FLAGS_RESERVED     (0x04)      /**< reserved */
#define IPV4_HDR_FLAGS_DF           (0x02)      /**< do not fragment */
#define IPV4_HDR_FLAGS_MF           (0x01)      /**< more fragments */
/** @} */

/**
 * @brief   Data type to represent an IPv4 packet header.
 *
 * The structure of the header is as follows:
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
     * @brief   Version and Internet Header Length.
     *
     * The version is encoded in the four most significant bits and the
     * Internet Header Length in the four least significant bits (see above).
     *
     * The Internet Header Length is the length of the header in 32-bit words,
     * so it must be multiplied by 4 to get the length in bytes. The minimum
     * value is 5 (20 bytes) and the maximum is 15 (60 bytes).
     *
     * This module provides helper functions to set, get, and check these
     * fields accordingly:
     *
     * * ipv4_hdr_set_version()
     * * ipv4_hdr_get_version()
     * * ipv4_hdr_set_ihl()
     * * ipv4_hdr_get_ihl()
     */
    uint8_t v_ihl;
    uint8_t ts;             /**< type of service of packet */
    network_uint16_t tl;    /**< total length of the datagram */
    network_uint16_t id;    /**< identification value of packet */
    /**
     * @brief   Version control flags and Fragment Offset.
     *
     * The flags are the 3 most significant bits, and the remaining 13 bits are
     * the fragment offset.
     *
     * This module provides helper functions to set, get, and check these
     * fields accordingly:
     *
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
 * @brief   Sets the version field of @p hdr to 4
 *
 * @param[out]  hdr     Pointer to an IPv4 header.
 */
static inline void ipv4_hdr_set_version(ipv4_hdr_t *hdr)
{
    hdr->v_ihl &= 0x0f;
    hdr->v_ihl |= 0x40;
}

/**
 * @brief   Gets the value of the version field of @p hdr
 *
 * @param[in]   hdr     Pointer to an IPv4 header.
 *
 * @return  Value of the version field of @p hdr.
 */
static inline uint8_t ipv4_hdr_get_version(ipv4_hdr_t *hdr)
{
    return ((hdr->v_ihl) >> 4);
}

/**
 * @brief   Sets the Internet Header Length field of @p hdr
 *
 * @param[out]  hdr     Pointer to an IPv4 header.
 * @param[in]   ihl     Size in bytes of the Internet Header Length (including padding)
 */
static inline void ipv4_hdr_set_ihl(ipv4_hdr_t *hdr, uint16_t ihl)
{
    assert(ihl >= 20 && ihl <= 60);
    assert(ihl % 4 == 0);

    hdr->v_ihl &= 0xf0;
    hdr->v_ihl |= 0x0f & (ihl >> 2);
}

/**
 * @brief   Gets the value of the Internet Header Length field of @p hdr
 *
 * @param[in]   hdr     Pointer to an IPv4 header.
 *
 * @return Size in bytes of the Internet Header Length field of @p hdr
 */
static inline uint16_t ipv4_hdr_get_ihl(ipv4_hdr_t *hdr)
{
    return (hdr->v_ihl & 0x0f) << 2;
}

/**
 * @brief   Sets the Version Control Flags field of @p hdr
 *
 * @param[out]  hdr     Pointer to an IPv4 header.
 * @param[in]   flags   The new value of flags
 */
static inline void ipv4_hdr_set_flags(ipv4_hdr_t *hdr, uint8_t flags)
{
    hdr->fl_fo.u8[0] &= 0x1f;
    hdr->fl_fo.u8[0] |= (0xe0 & (flags << 5));
}

/**
 * @brief   Gets the value of the Version Control Flags field of @p hdr
 *
 * @param[in]   hdr     Pointer to an IPv4 header.
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
 * @param[out]  hdr     Pointer to an IPv4 header.
 * @param[in]   fo      The new value of fragment offset
 */
static inline void ipv4_hdr_set_fo(ipv4_hdr_t *hdr, uint16_t fo)
{
    hdr->fl_fo.u8[0] &= 0xe0;
    hdr->fl_fo.u8[0] |= (0x1f & (fo >> 8));
    hdr->fl_fo.u8[1] = 0xff & fo;
}

/**
 * @brief   Gets the value of the Fragment Offset field of @p hdr
 *
 * @param[in]   hdr     Pointer to an IPv4 header.
 *
 * @return Value of the Fragment Offset field of @p hdr
 */
static inline uint16_t ipv4_hdr_get_fo(ipv4_hdr_t *hdr)
{
    return (((hdr->fl_fo.u8[0] & 0x1f) << 8) + hdr->fl_fo.u8[1]);
}

/**
 * @brief   Calculates the header checksum of @p hdr.
 *
 * @see     [RFC 791, section 3.1](https://tools.ietf.org/html/rfc791#section-3.1)
 *
 * @pre     ipv4_hdr_t::csum of @p hdr is set to 0.
 *
 * @param[in] hdr   An IPv4 header, with ipv4_hdr_t::csum set to 0.
 *
 * @return  The IPv4 header checksum of @p hdr, in network byte order.
 */
static inline network_uint16_t ipv4_hdr_csum(ipv4_hdr_t *hdr)
{
    uint16_t csum = inet_csum(0, (uint8_t *)hdr, ipv4_hdr_get_ihl(hdr));

    return byteorder_htons(~csum);
}

/**
 * @brief   Calculates the Internet Checksum for the IPv4 Pseudo Header.
 *
 * @see     [RFC 793, section 3.1](https://tools.ietf.org/html/rfc793#section-3.1)
 *
 * @param[in] sum       Preinitialized value of the sum.
 * @param[in] prot_num  The @ref net_protnum you want to calculate the
 *                      checksum for.
 * @param[in] hdr       An IPv4 header to derive the Pseudo Header from.
 * @param[in] len       The upper-layer packet length for the pseudo header.
 *
 * @return  The non-normalized Internet Checksum of the given IPv4 pseudo header.
 */
static inline uint16_t ipv4_hdr_inet_csum(uint16_t sum, ipv4_hdr_t *hdr,
                                          uint8_t prot_num, uint16_t len)
{
    if (((uint32_t)sum + len + prot_num) > 0xffff) {
        /* increment by one for overflow to keep it as 1's complement sum */
        sum++;
    }

    return inet_csum(sum + len + prot_num, hdr->src.u8,
                     (2 * sizeof(ipv4_addr_t)));
}

/**
 * @brief   Outputs an IPv4 header to stdout.
 *
 * @param[in] hdr   An IPv4 header.
 */
void ipv4_hdr_print(ipv4_hdr_t *hdr);

#ifdef __cplusplus
}
#endif

/** @} */
