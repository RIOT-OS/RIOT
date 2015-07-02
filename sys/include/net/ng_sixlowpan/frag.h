/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_ng_sixlowpan_frag   6LoWPAN Fragmentation
 * @ingroup     net_ng_sixlowpan
 * @brief       6LoWPAN Fragmentation headers and functionality
 * @see <a href="https://tools.ietf.org/html/rfc4944#section-5.3">
 *          RFC 4944, section 5.3
 *      </a>
 * @{
 *
 * @file
 * @brief   6LoWPAN Fragmentation definitions
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef NG_SIXLOWPAN_FRAG_H_
#define NG_SIXLOWPAN_FRAG_H_

#include <inttypes.h>
#include <stdbool.h>

#include "byteorder.h"
#include "kernel_types.h"
#include "net/ng_pkt.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NG_SIXLOWPAN_FRAG_DISP_MASK     (0xf8)      /**< mask for fragmentation
                                                     *   dispatch */
#define NG_SIXLOWPAN_FRAG_1_DISP        (0xc0)      /**< dispatch for 1st fragment */
#define NG_SIXLOWPAN_FRAG_N_DISP        (0xe0)      /**< dispatch for subsequent
                                                     *   fragments */
#define NG_SIXLOWPAN_FRAG_SIZE_MASK     (0x07ff)    /**< mask for datagram size */

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
} ng_sixlowpan_frag_t;

/**
 * @brief   Subsequent 6LoWPAN fragmentation header
 *
 * @see <a href="https://tools.ietf.org/html/rfc4944#section-5.3">
 *          RFC 4944, section 5.3
 *      </a>
 *
 * @extends ng_sixlowpan_frag_t
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
} ng_sixlowpan_frag_n_t;

/**
 * @brief   Checks if a given fragment is a 6LoWPAN fragment.
 *
 * @param[in] hdr   A 6LoWPAN fragmentation header.
 *
 * @return  true, if given fragment is a 6LoWPAN fragment.
 * @return  false, if given fragment is not a 6LoWPAN fragment.
 */
static inline bool ng_sixlowpan_frag_is(ng_sixlowpan_frag_t *hdr)
{
    return ((hdr->disp_size.u8[0] & NG_SIXLOWPAN_FRAG_DISP_MASK) ==
            NG_SIXLOWPAN_FRAG_1_DISP) ||
           ((hdr->disp_size.u8[0] & NG_SIXLOWPAN_FRAG_DISP_MASK) ==
            NG_SIXLOWPAN_FRAG_N_DISP);
}

/**
 * @brief   Sends a packet fragmented.
 *
 * @param[in] pid           The interface to send the packet over.
 * @param[in] pkt           The packet to send.
 * @param[in] payload_len   The length of the payload to send (IPv6 packet size
 *                          + inner 6LoWPAN dispatches).
 *                          This value is purely given to not calculate the
 *                          payload length using @ref ng_pkt_len() repeatedly.
 * @param[in] datagram_size The length of just the IPv6 packet. It is the value
 *                          set that the ng_sixlowpan_frag_t::disp_size will be
 *                          set to.
 */
void ng_sixlowpan_frag_send(kernel_pid_t pid, ng_pktsnip_t *pkt,
                            size_t payload_len, size_t datagram_size);

/**
 * @brief   Handles a packet containing a fragment header.
 *
 * @param[in] pkt   The packet to handle.
 */
void ng_sixlowpan_frag_handle_pkt(ng_pktsnip_t *pkt);

#ifdef __cplusplus
}
#endif

#endif /* NG_SIXLOWPAN_FRAG_H_ */
/** @} */
