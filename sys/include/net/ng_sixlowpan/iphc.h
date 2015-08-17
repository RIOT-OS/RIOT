/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_ng_sixlowpan_iphc   IPv6 header compression (IPHC)
 * @ingroup     net_ng_sixlowpan
 * @brief       IPv6 header compression for 6LoWPAN.
 * @{
 *
 * @file
 * @brief   6LoWPAN IPHC definitions
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef NG_SIXLOWPAN_IPHC_H_
#define NG_SIXLOWPAN_IPHC_H_

#include <stdbool.h>

#include "net/ng_pkt.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Dispatch mask for LOWPAN_IPHC.
 */
#define NG_SIXLOWPAN_IPHC1_DISP_MASK    (0xe0)

/**
 * @brief   Dispatch for LOWPAN_IPHC.
 */
#define NG_SIXLOWPAN_IPHC1_DISP         (0x60)

/**
 * @brief   Flag for Traffic Class & Flow Label elision (part of first byte of
 *          LOWPAN_IPHC).
 * @see <a href="http://tools.ietf.org/html/rfc6282#section-3.1.1">
 *          RFC 6282, section 3.1.1
 *      </a>
 */
#define NG_SIXLOWPAN_IPHC1_TF           (0x18)

/**
 * @brief   Flag for Next Header Compression (part of first byte of
 *          LOWPAN_IPHC).
 * @see <a href="http://tools.ietf.org/html/rfc6282#section-3.1.1">
 *          RFC 6282, section 3.1.1
 *      </a>
 */
#define NG_SIXLOWPAN_IPHC1_NH           (0x04)

/**
 * @brief   Flag for Hop Limit elision (part of first byte of LOWPAN_IPHC).
 * @see <a href="http://tools.ietf.org/html/rfc6282#section-3.1.1">
 *          RFC 6282, section 3.1.1
 *      </a>
 */
#define NG_SIXLOWPAN_IPHC1_HL           (0x03)

/**
 * @brief   Flag for Context Identifier Extention (part of second byte
 *          of LOWPAN_IPHC).
 * @see <a href="http://tools.ietf.org/html/rfc6282#section-3.1.1">
 *          RFC 6282, section 3.1.1
 *      </a>
 */
#define NG_SIXLOWPAN_IPHC2_CID_EXT      (0x80)

/**
 * @brief   Flag for Source Address Compression (part of second byte
 *          of LOWPAN_IPHC).
 * @see <a href="http://tools.ietf.org/html/rfc6282#section-3.1.1">
 *          RFC 6282, section 3.1.1
 *      </a>
 */
#define NG_SIXLOWPAN_IPHC2_SAC          (0x40)

/**
 * @brief   Bits for Source Address Mode (part of second byte of
 *          LOWPAN_IPHC).
 * @see <a href="http://tools.ietf.org/html/rfc6282#section-3.1.1">
 *          RFC 6282, section 3.1.1
 *      </a>
 */
#define NG_SIXLOWPAN_IPHC2_SAM          (0x30)

/**
 * @brief   Flag for Destination Address Compression (part of second
 *          byte of LOWPAN_IPHC).
 * @see <a href="http://tools.ietf.org/html/rfc6282#section-3.1.1">
 *          RFC 6282, section 3.1.1
 *      </a>
 */
#define NG_SIXLOWPAN_IPHC2_DAC          (0x04)

/**
 * @brief   Bits for Destination Address Mode (part of second byte of
 *          LOWPAN_IPHC).
 * @see <a href="http://tools.ietf.org/html/rfc6282#section-3.1.1">
 *          RFC 6282, section 3.1.1
 *      </a>
 */
#define NG_SIXLOWPAN_IPHC2_DAM          (0x03)

/**
 * @brief   Flag for Multicast Compression (part of second byte of
 *          LOWPAN_IPHC).
 * @see <a href="http://tools.ietf.org/html/rfc6282#section-3.1.1">
 *          RFC 6282, section 3.1.1
 *      </a>
 */
#define NG_SIXLOWPAN_IPHC2_M            (0x08)

/**
 * @brief   6LoWPAN IPHC header length
 */
#define NG_SIXLOWPAN_IPHC_HDR_LEN       (2)

/**
 * @brief   6LoWPAN context idendifier extension header length
 */
#define NG_SIXLOWPAN_IPHC_CID_EXT_LEN   (1)

/**
 * @brief   Checks if datagram is an IPHC datagram.
 *
 * @param[in] data  Data of a datagram, may not be NULL.
 *
 * @return  true, if datagram is an IPHC datagram.
 * @return  false, if datagram is not an IPHC datagram.
 */
static inline bool ng_sixlowpan_iphc_is(uint8_t *data)
{
    return ((*data & NG_SIXLOWPAN_IPHC1_DISP_MASK) == NG_SIXLOWPAN_IPHC1_DISP);
}

/**
 * @brief   Decompresses a received 6LoWPAN IPHC frame.
 *
 * @pre (ipv6 != NULL) && (ipv6->size >= sizeof(ng_ipv6_hdr_t))
 *
 * @param[out] ipv6     A pre-allocated IPv6 header. Will not be inserted into
 *                      @p pkt
 * @param[in,out] pkt   A received 6LoWPAN IPHC frame. IPHC dispatch will not
 *                      be marked.
 * @param[in] size      Offset of the IPHC dispatch in 6LoWPaN frame.
 *
 * @return  length of the HC dispatches + inline values on success.
 * @return  0 on error.
 */
size_t ng_sixlowpan_iphc_decode(ng_pktsnip_t *ipv6, ng_pktsnip_t *pkt, size_t offset);

/**
 * @brief   Compresses a 6LoWPAN for IPHC.
 *
 * @param[in,out] pkt   A 6LoWPAN frame with an uncompressed IPv6 header to
 *                      send. Will be translated to an 6LoWPAN IPHC frame.
 *
 * @return  true, on success
 * @return  false, on error.
 */
bool ng_sixlowpan_iphc_encode(ng_pktsnip_t *pkt);

#ifdef __cplusplus
}
#endif

#endif /* NG_SIXLOWPAN_IPHC_H_ */
/** @} */
