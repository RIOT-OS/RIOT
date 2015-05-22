/*
 * Copyright (C) 2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_sixlowpan_lowpan
 * @{
 *
 * @file
 * @brief       6LoWPAN error codes
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#ifndef SIXLOWPAN_ERROR_H
#define SIXLOWPAN_ERROR_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Functions return this if call was success. Only defined if not
 * already defined by other header.
 */
#define SIXLOWERROR_SUCCESS                 (0)

/**
 * Error code that signals that array is full.
 */
#define SIXLOWERROR_ARRAYFULL   (132)

/**
 * Error code that signals a NULL pointer error.
 */
#define SIXLOWERROR_NULLPTR     (133)

/**
 * Error code that signals an illegal value.
 */
#define SIXLOWERROR_VALUE       (134)

/**
 * Error code that signals an illegal address.
 */
#define SIXLOWERROR_ADDRESS     (135)

/**
 * Error code that signals an unknown 6LoWPAN dispatch
 * @see <a href="http://tools.ietf.org/html/rfc4944#section-5">
 *          RFC 4944, Section 5: LoWPAN Adaptation Layer and Frame Format
 *      </a>
 */
#define SIXLOWERROR_DISPATCH    (136)

/**
 * Error code that signals that the first 6LoWPAN fragment was
 * not received
 * @see <a href="http://tools.ietf.org/html/rfc4944#section-5.3">
 *          RFC 4944, Section 5.3: Fragmentation Type and Header
 *      </a>
 */
#define SIXLOWERROR_FSTFRAG     (137)

/**
 * Error code that signals that an invalid 6LoWPAN fragment was
 * received.
 * @see <a href="http://tools.ietf.org/html/rfc4944#section-5.3">
 *          RFC 4944, Section 5.3: Fragmentation Type and Header
 *      </a>
 */
#define SIXLOWERROR_INVFRAG     (138)

/**
 * Error code that signals that the source context identifier was not
 * found.
 * @see <a href="http://tools.ietf.org/html/rfc6282#section-3.1">
 *          RFC 6282, Section 3.1: LOWPAN_IPHC Encoding format
 *      </a>
 */
#define SIXLOWERROR_SCI         (139)

/**
 * Error code that signals that the destination context identifier was
 * not found.
 * @see <a href="http://tools.ietf.org/html/rfc6282#section-3.1">
 *          RFC 6282, Section 3.1: LOWPAN_IPHC Encoding format
 *      </a>
 */
#define SIXLOWERROR_DCI         (140)

/**
 * Error code that signals that the destination context identifier was
 * not found.
 */
#define SIXLOWERROR_CSUM        (141)

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* SIXLOWPAN_ERROR_H */
