/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_sixlowpan   6LoWPAN
 * @ingroup     net
 * @brief       Provides 6LoWPAN dispatch types and helper functions
 * @{
 *
 * @file
 * @brief   6LoWPAN dispatch type and helper function definitions.
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef NET_SIXLOWPAN_H
#define NET_SIXLOWPAN_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "byteorder.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    6LoWPAN dispatch definition
 * @see     <a href="https://tools.ietf.org/html/rfc4944#section-5.1">
 *              RFC 4944, section 5.1
 *          </a>
 * @{
 */
#define SIXLOWPAN_UNCOMP            (0x41)      /**< uncompressed 6LoWPAN frame dispatch. */
#define SIXLOWPAN_FRAG_DISP_MASK    (0xf8)      /**< mask for fragmentation
                                                 *   dispatch */
#define SIXLOWPAN_FRAG_1_DISP       (0xc0)      /**< dispatch for 1st fragment */
#define SIXLOWPAN_FRAG_N_DISP       (0xe0)      /**< dispatch for subsequent
                                                 *   fragments */
#define SIXLOWPAN_FRAG_MAX_LEN      (2047)      /**< Maximum datagram size @f$ (2^{11} - 1) @f$ */

/**
 * @brief   Dispatch mask for LOWPAN_IPHC.
 * @see     <a href="https://tools.ietf.org/html/rfc6282#section-3.1">
 *              RFC 6282, section 3.1
 *          </a>
 */
#define SIXLOWPAN_IPHC1_DISP_MASK   (0xe0)

/**
 * @brief   Dispatch for LOWPAN_IPHC.
 * @see     <a href="https://tools.ietf.org/html/rfc6282#section-3.1">
 *              RFC 6282, section 3.1
 *          </a>
 */
#define SIXLOWPAN_IPHC1_DISP        (0x60)

/**
 * @brief   Dispatch mask for 6LoWPAN selective fragment recovery
 * @see [RFC 8931, section 5](https://tools.ietf.org/html/rfc8931#section-5)
 */
#define SIXLOWPAN_SFR_DISP_MASK         (0xfe)

/**
 * @brief   Dispatch for 6LoWPAN recoverable fragment
 * @see [RFC 8931, section 5.1](https://tools.ietf.org/html/rfc8931#section-5.1)
 */
#define SIXLOWPAN_SFR_RFRAG_DISP        (0xe8)

/**
 * @brief   Dispatch for 6LoWPAN recoverable fragment acknowledgment
 * @see [RFC 8931, section 5.2](https://tools.ietf.org/html/rfc8931#section-5.2)
 */
#define SIXLOWPAN_SFR_ACK_DISP          (0xea)

/**
 * @brief   Checks if dispatch indicates that frame is not a 6LoWPAN (NALP) frame.
 *
 * @param[in] disp  The first byte of a frame.
 *
 * @return  true, if frame is a NALP.
 * @return  false, if frame is not a NALP.
 */
static inline bool sixlowpan_nalp(uint8_t disp)
{
    return ((disp & 0xc0) == 0);
}
/** @} */

/**
 * @name    6LoWPAN fragmentation header definitions
 * @{
 */
#define SIXLOWPAN_FRAG_SIZE_MASK    (0x07ff)    /**< mask for datagram size */

/**
 * @brief   General and 1st 6LoWPAN fragmentation header
 *
 * @note    The general 6LoWPAN fragmentation header refers to the first 4
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
} sixlowpan_frag_t;

/**
 * @brief   Subsequent 6LoWPAN fragmentation header
 *
 * @see <a href="https://tools.ietf.org/html/rfc4944#section-5.3">
 *          RFC 4944, section 5.3
 *      </a>
 *
 * @extends sixlowpan_frag_t
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
} sixlowpan_frag_n_t;

/**
 * @brief   Checks if a given header is a 1st 6LoWPAN fragment header
 *
 * @param[in] hdr   A 6LoWPAN fragmentation header.
 *
 * @return  true, if given fragment is a 1st 6LoWPAN fragment.
 * @return  false, if given fragment is not a 1st 6LoWPAN fragment.
 */
static inline bool sixlowpan_frag_1_is(sixlowpan_frag_t *hdr)
{
    return ((hdr->disp_size.u8[0] & SIXLOWPAN_FRAG_DISP_MASK) == SIXLOWPAN_FRAG_1_DISP);
}

/**
 * @brief   Checks if a given header is a subsequent 6LoWPAN fragment header
 *
 * @param[in] hdr   A 6LoWPAN fragmentation header.
 *
 * @return  true, if given fragment is a subsequent 6LoWPAN fragment.
 * @return  false, if given fragment is not a subsequent 6LoWPAN fragment.
 */
static inline bool sixlowpan_frag_n_is(sixlowpan_frag_t *hdr)
{
    return ((hdr->disp_size.u8[0] & SIXLOWPAN_FRAG_DISP_MASK) == SIXLOWPAN_FRAG_N_DISP);
}

/**
 * @brief   Checks if a given header is a 6LoWPAN fragment header.
 *
 * @param[in] hdr   A 6LoWPAN fragmentation header.
 *
 * @return  true, if given fragment is a 6LoWPAN fragment.
 * @return  false, if given fragment is not a 6LoWPAN fragment.
 */
static inline bool sixlowpan_frag_is(sixlowpan_frag_t *hdr)
{
    return sixlowpan_frag_1_is(hdr) || sixlowpan_frag_n_is(hdr);
}

/**
 * @brief   Get datagram size from general 6LoWPAN fragment header
 *
 * @param[in] hdr   A general 6LoWPAN fragment header.
 *
 * @return  The datagram size for the 6LoWPAN fragment.
 */
static inline uint16_t sixlowpan_frag_datagram_size(sixlowpan_frag_t *hdr)
{
    return (byteorder_ntohs(hdr->disp_size) & SIXLOWPAN_FRAG_SIZE_MASK);
}

/**
 * @brief   Get datagram tag from general 6LoWPAN fragment header
 *
 * @param[in] hdr   A general 6LoWPAN fragment header.
 *
 * @return  The datagram tag for the 6LoWPAN fragment.
 */
static inline uint16_t sixlowpan_frag_datagram_tag(sixlowpan_frag_t *hdr)
{
    return byteorder_ntohs(hdr->tag);
}

/**
 * @brief   Get fragment offset from a subsequent 6LoWPAN fragment header
 *
 * @param[in] hdr   A subsequent 6LoWPAN fragment header.
 *
 * @return  The offset of the 6LoWPAN fragment.
 */
static inline uint16_t sixlowpan_frag_offset(sixlowpan_frag_n_t *hdr)
{
    /* https://tools.ietf.org/html/rfc4944#section-5.3:
     * datagram_offset:  This field is present only in the second and
     *    subsequent link fragments and SHALL specify the offset, in
     *    increments of 8 octets, of the fragment from the beginning of the
     *    payload datagram. [...] */
    return (hdr->offset * 8U);
}
/** @} */

/**
 * @name    6LoWPAN IPHC dispatch definitions
 * @{
 */
/**
 * @brief   Flag for Traffic Class & Flow Label elision (part of first byte of
 *          LOWPAN_IPHC).
 * @see <a href="http://tools.ietf.org/html/rfc6282#section-3.1.1">
 *          RFC 6282, section 3.1.1
 *      </a>
 */
#define SIXLOWPAN_IPHC1_TF          (0x18)

/**
 * @brief   Flag for Next Header Compression (part of first byte of
 *          LOWPAN_IPHC).
 * @see <a href="http://tools.ietf.org/html/rfc6282#section-3.1.1">
 *          RFC 6282, section 3.1.1
 *      </a>
 */
#define SIXLOWPAN_IPHC1_NH          (0x04)

/**
 * @brief   Flag for Hop Limit elision (part of first byte of LOWPAN_IPHC).
 * @see <a href="http://tools.ietf.org/html/rfc6282#section-3.1.1">
 *          RFC 6282, section 3.1.1
 *      </a>
 */
#define SIXLOWPAN_IPHC1_HL          (0x03)

/**
 * @brief   Flag for Context Identifier Extension (part of second byte
 *          of LOWPAN_IPHC).
 * @see <a href="http://tools.ietf.org/html/rfc6282#section-3.1.1">
 *          RFC 6282, section 3.1.1
 *      </a>
 */
#define SIXLOWPAN_IPHC2_CID_EXT     (0x80)

/**
 * @brief   Flag for Source Address Compression (part of second byte
 *          of LOWPAN_IPHC).
 * @see <a href="http://tools.ietf.org/html/rfc6282#section-3.1.1">
 *          RFC 6282, section 3.1.1
 *      </a>
 */
#define SIXLOWPAN_IPHC2_SAC         (0x40)

/**
 * @brief   Bits for Source Address Mode (part of second byte of
 *          LOWPAN_IPHC).
 * @see <a href="http://tools.ietf.org/html/rfc6282#section-3.1.1">
 *          RFC 6282, section 3.1.1
 *      </a>
 */
#define SIXLOWPAN_IPHC2_SAM         (0x30)

/**
 * @brief   Flag for Destination Address Compression (part of second
 *          byte of LOWPAN_IPHC).
 * @see <a href="http://tools.ietf.org/html/rfc6282#section-3.1.1">
 *          RFC 6282, section 3.1.1
 *      </a>
 */
#define SIXLOWPAN_IPHC2_DAC         (0x04)

/**
 * @brief   Bits for Destination Address Mode (part of second byte of
 *          LOWPAN_IPHC).
 * @see <a href="http://tools.ietf.org/html/rfc6282#section-3.1.1">
 *          RFC 6282, section 3.1.1
 *      </a>
 */
#define SIXLOWPAN_IPHC2_DAM         (0x03)

/**
 * @brief   Flag for Multicast Compression (part of second byte of
 *          LOWPAN_IPHC).
 * @see <a href="http://tools.ietf.org/html/rfc6282#section-3.1.1">
 *          RFC 6282, section 3.1.1
 *      </a>
 */
#define SIXLOWPAN_IPHC2_M           (0x08)

/**
 * @brief   6LoWPAN IPHC header length
 */
#define SIXLOWPAN_IPHC_HDR_LEN      (2)

/**
 * @brief   6LoWPAN context identifier extension header length
 */
#define SIXLOWPAN_IPHC_CID_EXT_LEN  (1)

/**
 * @brief   Checks if datagram is an IPHC datagram.
 *
 * @param[in] data  Data of a datagram. Must not be NULL.
 *
 * @return  true, if datagram is an IPHC datagram.
 * @return  false, if datagram is not an IPHC datagram.
 */
static inline bool sixlowpan_iphc_is(uint8_t *data)
{
    return ((*data & SIXLOWPAN_IPHC1_DISP_MASK) == SIXLOWPAN_IPHC1_DISP);
}
/** @} */

/**
 * @brief   Prints 6LoWPAN dispatch to stdout.
 *
 * @param[in] data  A 6LoWPAN frame.
 * @param[in] size  Size of @p data.
 */
void sixlowpan_print(uint8_t *data, size_t size);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* NET_SIXLOWPAN_H */
