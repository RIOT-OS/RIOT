/*
 * Copyright (C) 2017 HAW Hamburg
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_lowpan      LoWPAN
 * @ingroup     net
 * @brief       Provides LoWPAN dispatch types and helper functions
 * @{
 *
 * @file
 * @brief   LoWPAN dispatch type and helper function definitons.
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 * @author  Cenk Gündoğan <mail-github@cgundogan.de>
 */
#ifndef NET_LOWPAN_H
#define NET_LOWPAN_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "byteorder.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    LoWPAN dispatch definition
 * @see     <a href="https://tools.ietf.org/html/rfc4944#section-5.1">
 *              RFC 4944, section 5.1
 *          </a>
 * @{
 */
#define LOWPAN_UNCOMP_IPV6          (0x41)      /**< uncompressed IPv6 header dispatch. */
#define LOWPAN_FRAG_DISP_MASK       (0xf8)      /**< mask for fragmentation
                                                 *   dispatch */
#define LOWPAN_FRAG_1_DISP          (0xc0)      /**< dispatch for 1st fragment */
#define LOWPAN_FRAG_N_DISP          (0xe0)      /**< dispatch for subsequent
                                                 *   fragments */
#define LOWPAN_FRAG_MAX_LEN         (2047)      /**< Maximum datagram size @f$ (2^{11} - 1) @f$ */

/**
 * @brief   Checks if dispatch indicates that frame is not a LoWPAN (NALP) frame.
 *
 * @param[in] disp  The first byte of a frame.
 *
 * @return  true, if frame is a NALP.
 * @return  false, if frame is not a NALP.
 */
static inline bool lowpan_nalp(uint8_t disp)
{
    return ((disp & 0xc0) == 0);
}
/** @} */

/**
 * @name    LoWPAN fragmentation header definitions
 * @{
 */
#define LOWPAN_FRAG_SIZE_MASK       (0x07ff)    /**< mask for datagram size */

/**
 * @brief   General and 1st LoWPAN fragmentation header
 *
 * @note    The general LoWPAN fragmentation header refers to the first 4
 *          bytes of a \c FRAG0 or \c FRAGN fragmentation header, which are
 *          identical.
 *
 * @see <a href="https://tools.ietf.org/html/rfc4944#section-5.3">
 *          RFC 4944, section 5.3
 *      </a>
 */
typedef struct __attribute__((packed)) {
    /**
     * @brief   Dispatch and datagram size.
     *
     * @details The 5 most significant bits are the dispatch, the remaining
     *          bits are the size.
     */
    network_uint16_t disp_size;
    network_uint16_t tag;       /**< datagram tag */
} lowpan_frag_t;

/**
 * @brief   Subsequent LoWPAN fragmentation header
 *
 * @see <a href="https://tools.ietf.org/html/rfc4944#section-5.3">
 *          RFC 4944, section 5.3
 *      </a>
 *
 * @extends lowpan_frag_t
 */
typedef struct __attribute__((packed)) {
    /**
     * @brief   Dispatch and datagram size.
     *
     * @details The 5 most significant bits are the dispatch, the remaining
     *          bits are the size.
     */
    network_uint16_t disp_size;
    network_uint16_t tag;       /**< datagram tag */
    uint8_t offset;             /**< offset */
} lowpan_frag_n_t;

/**
 * @brief   Checks if a given fragment is a LoWPAN fragment.
 *
 * @param[in] hdr   A LoWPAN fragmentation header.
 *
 * @return  true, if given fragment is a LoWPAN fragment.
 * @return  false, if given fragment is not a LoWPAN fragment.
 */
static inline bool lowpan_frag_is(lowpan_frag_t *hdr)
{
    return ((hdr->disp_size.u8[0] & LOWPAN_FRAG_DISP_MASK) ==
            LOWPAN_FRAG_1_DISP) ||
           ((hdr->disp_size.u8[0] & LOWPAN_FRAG_DISP_MASK) ==
            LOWPAN_FRAG_N_DISP);
}
/** @} */

/**
 * @brief   Prints LoWPAN dispatch to stdout.
 *
 * @param[in] data  A LoWPAN frame.
 * @param[in] size  Size of @p data.
 */
void lowpan_print(uint8_t *data, size_t size);

#ifdef __cplusplus
}
#endif

#endif /* NET_LOWPAN_H */
/** @} */
