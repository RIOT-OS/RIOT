/*
 * Copyright (C) 2015 José Ignacio Alamos <jialamos@uc.cl>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

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
#ifndef NET_IPV4_HDR_H
#define NET_IPV4_HDR_H

#include "byteorder.h"
#include "net/inet_csum.h"
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
     * @details The flags are the 3 most significant bits, and the remaining 13 bits are the fragment offfset
     *
     * This module provides helper functions to set, get, and check these
     * fields accordingly:
     * * ipv4_hdr_is()
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
static inline uint8_t ipv4_hdr_get_version(const ipv4_hdr_t *hdr)
{
    return ((hdr->v_ih) >> 4);
}

/**
 * @brief   Checks if the version field is set to 4
 *
 * @param[in] hdr   Pointer to an IPv4 header.
 *
 * @return  true, if version field is 4
 * @return  false, otherwise
 */
static inline bool ipv4_hdr_is(const ipv4_hdr_t *hdr)
{
    return ipv4_hdr_get_version(hdr) == 0x04;
}

/**
 * @brief   Sets the Internet Header Length field of @p hdr
 *
 * @param[out] hdr  Pointer to an IPv4 header.
 * @param[in] ihl  Size in bits of the Internet Header Length (including padding)
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
 * @return Size in bites of the Internet Header Length field of @p hdr
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

/**
 * @brief   Calculates the Internet Checksum for the IPv4 Pseudo Header.
 *
 * @param[in] hdr       An IPv4 header
 */
static inline void ipv4_hdr_inet_csum(ipv4_hdr_t *hdr)
{
    uint16_t csum;

    // Ensure the checksum is 0, to be able to count it ;p
    hdr->csum = byteorder_htons(0);

    csum = inet_csum(0, (uint8_t *) hdr, sizeof(ipv4_hdr_t));
    hdr->csum = byteorder_htons(~csum);
}

static inline uint16_t ipv4_hdr_inet_csum_prot(uint16_t sum, ipv4_hdr_t *hdr,
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

#endif /* NET_IPV4_HDR_H */
/** @} */
