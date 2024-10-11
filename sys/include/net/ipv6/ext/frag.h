/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_ipv6_ext_frag   IPv6 fragmentation extension
 * @ingroup     net_ipv6_ext
 * @brief       Definitions for IPv6 fragmentation extension
 * @see         [RFC 8200, section 4.5](https://tools.ietf.org/html/rfc8200#section-4.5)
 * @{
 *
 * @file
 * @brief   Fragmentation extension definitions
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef NET_IPV6_EXT_FRAG_H
#define NET_IPV6_EXT_FRAG_H

#include <stdbool.h>
#include <stdint.h>

#include "byteorder.h"

#ifdef __cplusplus
extern "C" {
#endif

#define IPV6_EXT_FRAG_OFFSET_MASK   (0xFFF8)    /**< Mask for the offset */
#define IPV6_EXT_FRAG_M             (0x0001)    /**< M flag */

/**
 * @brief   Fragment header definition
 */
typedef struct __attribute__((packed)) {
    uint8_t nh;                     /**< next header */
    uint8_t resv;                   /**< reserved */
    /**
     * @brief   11-bit fragment offset and flags
     *
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     * |      Fragment Offset    |Res|M|
     * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     */
    network_uint16_t offset_flags;
    network_uint32_t id;            /**< identification */
} ipv6_ext_frag_t;

/**
 * @brief   Get offset of fragment in bytes
 *
 * @param[in] frag  A fragment header
 *
 * @return  Offset of fragment in bytes.
 */
static inline unsigned ipv6_ext_frag_get_offset(const ipv6_ext_frag_t *frag)
{
    /* The offset is left-shifted by 3 bytes in the header * (equivalent to
     * multiplication with 8), and the offset is in units 8 bytes
     * so no shifting or multiplying necessary to get offset in bytes */
    return (byteorder_ntohs(frag->offset_flags) & IPV6_EXT_FRAG_OFFSET_MASK);
}

/**
 * @brief   Checks if more fragments are coming after the given fragment
 *
 * @param[in] frag  A fragment header
 *
 * @return  true, when more fragments are coming after the given fragment.
 * @return  false, when the given fragment is the last.
 */
static inline bool ipv6_ext_frag_more(const ipv6_ext_frag_t *frag)
{
    return (byteorder_ntohs(frag->offset_flags) & IPV6_EXT_FRAG_M);
}

/**
 * @brief   Sets the offset field of a fragment header
 *
 * @note    Must be called before @ref ipv6_ext_frag_set_more()
 *
 * @param[in,out] frag  A fragment header
 * @param[in] offset    The offset of the fragment in bytes.
 *                      Is assumed to be a multiple of 8.
 *                      Is assumed to be lesser or equal to 65528.
 */
static inline void ipv6_ext_frag_set_offset(ipv6_ext_frag_t *frag,
                                            unsigned offset)
{
    /* The offset is left-shifted by 3 bytes in the header * (equivalent to
     * multiplication with 8), and the offset is a multiple of 8 bytes
     * so no shifting or division necessary to set offset in units of 8 bytes */
    frag->offset_flags = byteorder_htons(offset & IPV6_EXT_FRAG_OFFSET_MASK);
}

/**
 * @brief   Sets the M flag of a fragment header
 *
 * @note    Must be called after @ref ipv6_ext_frag_set_offset()
 *
 * @param[in,out] frag      A fragment header
 */
static inline void ipv6_ext_frag_set_more(ipv6_ext_frag_t *frag)
{
    frag->offset_flags.u8[1] |= IPV6_EXT_FRAG_M;
}

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* NET_IPV6_EXT_FRAG_H */
