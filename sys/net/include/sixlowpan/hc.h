/*
 * Copyright (C) 2014 Martin Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @defgroup    net_sixlowpan_hc 6LoWPAN header compression
 * @addtogroup  net_sixlowpan
 * @{
 *
 * @file        sixlowpan/hc.h
 * @brief       Header compression as defined in
 *              <a href="https://tools.ietf.org/html/rfc6282">RFC 6282</a>
 *
 * @see         <a href="https://tools.ietf.org/html/rfc6282">
 *                  RFC 6282
 *              </a>
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */


#ifndef __SIXLOWPAN_HC_H_
#define __SIXLOWPAN_HC_H_

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
#define SIXLOWPAN_IPHC2_CID         (0x80)

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
#define SIXLOWPAN_IPHC_HDR_LEN          (2)

/**
 * @brief   6LoWPAN context idendifier extension header length
 */
#define SIXLOWPAN_IPHC_CONTEXT_HDR_LEN  (1)

/**
 * @brief   Initializes 6LoWPAN IPHC header statically with all values 0
 */
#define SIXLOWPAN_IPHC1_ZERO_INIT   {SIXLOWPAN_IPHC1_DISPATCH, 0, 0}

/**
 * @brief   Decodes header compression dispatch as defined into IPv6 headers
 *          and next headers (if implemented) as defined in
 *          <a href="https://tools.ietf.org/html/rfc6282">RFC 6282</a>
 *
 * @param[out] rcv      The resulting receive command for the netapi.
 * @param[in] src       Link layer source address in network byte-order.
 * @param[in] src_len   Length of *src* in byte. Currently, the only valid
 *                      values are 1, 2 and 8.
 * @param[in] dest      Link layer destination address network byte-order.
 * @param[in] dest_len  Length of *dest* in byte. Currently, the only valid
 *                      values are 1, 2 and 8.
 * @param[in] data      Payload from link layer.
 * @param[in] data_len  Length of *data* in byte.
 *
 * @return 0, on success
 * @return -EADDRNOTAVAIL, context encoded in given *data* was not found
 *                         in @ref iphc_cbuf
 * @return -EAFNOSUPPORT, *src_len* or *dest_len* was unequal to 1, 2 or 8
 * @return -EDESTADDRREQ, the compression encoding for the IPv6 destination
 *                        address in *data* was set to an illegal value
 * @return -EFAULT, *rcv*, *src*, *dest*, or *data* was NULL and DEVELHELP was defined
 * @return -EMSGSIZE, *data_len* is smaller then expected dispatch
 * @return -EINVAL, *data* was not HC encoded.
 */
int sixlowpan_hc_decode(netapi_rcv_pkt_t *rcv, void *src, size_t src_len,
                        void *dest, size_t dest_len, uint8_t *data,
                        size_t data_len);

/**
 * @brief   Encodes IPv6 and next headers (if implemented) into header
 *          compression dispatch as defined in
 *          <a href="https://tools.ietf.org/html/rfc6282">RFC 6282</a>
 *
 * @param[in,out] dispatch  The resulting dispatch for the netapi. Set the
 *                          maximum of available memory under
 *                          netdev_hlist_t::header in netdev_hlist_t::header_len
 *                          accordingly before calling this function.
 * @param[in,out] ulh       Upper layer headers. The headers from the upper
 *                          layers. The first one MUST be an IPv6 header.
 *                          This function removes the header it decodes into
 *                          *dispatch* but it makes a deep copy of the contents
 *                          of *ulh* first and returns this one then.
 * @param[in] src           Link layer source address in network byte-order
 * @param[in] src_len       Length of *src*. Currently, the only valid values
 *                          are 1, 2 and 8.
 * @param[in] dest          Link layer destination address in network byte-order.
 * @param[in] dest_len      Length of *dest*. Currently, the only valid values
 *                          are 1, 2 and 8.
 *
 * @return 0, on success
 * @return -EAFNOSUPPORT, *src_len* or *dest_len* was unequal to 1, 2, or 8
 * @return -EFAULT, *dispatch*, *src*, or *dest* was NULL and DEVELHELP was defined.
 * @return -EINVAL, *ulh* does not start with an IPv6 header.
 * @return -ENOMEM, space indicated in `dispatch->header_len` was not enough.
 */
int sixlowpan_hc_encode(netdev_hlist_t *dispatch, netdev_hlist_t **ulh,
                        void *src, size_t src_len, void *dest, size_t dest_len);

#endif /* __SIXLOWPAN_HC_H_ */
/**
 * @}
 */
