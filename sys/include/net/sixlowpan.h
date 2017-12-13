/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_sixlowpan   6LoWPAN
 * @ingroup     net_lowpan
 * @brief       Provides 6LoWPAN dispatch types and helper functions
 * @{
 *
 * @file
 * @brief   6LoWPAN dispatch type and helper function definitons.
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
/** @} */

/**
 * @name    LoWPAN IPHC dispatch definitions
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
 * @brief   Flag for Context Identifier Extention (part of second byte
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
 * @brief   LoWPAN IPHC header length
 */
#define SIXLOWPAN_IPHC_HDR_LEN      (2)

/**
 * @brief   6LoWPAN context idendifier extension header length
 */
#define SIXLOWPAN_IPHC_CID_EXT_LEN  (1)

/**
 * @brief   Checks if datagram is an IPHC datagram.
 *
 * @param[in] data  Data of a datagram, may not be NULL.
 *
 * @return  true, if datagram is an IPHC datagram.
 * @return  false, if datagram is not an IPHC datagram.
 */
static inline bool lowpan_iphc_is(uint8_t *data)
{
    return ((*data & SIXLOWPAN_IPHC1_DISP_MASK) == SIXLOWPAN_IPHC1_DISP);
}
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* NET_SIXLOWPAN_H */
/** @} */
