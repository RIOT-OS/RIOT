/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_gnrc_sixlowpan_frag
 * @{
 *
 * @file
 * @internal
 * @brief   6LoWPAN reassembly buffer
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef GNRC_SIXLOWPAN_FRAG_RBUF_H_
#define GNRC_SIXLOWPAN_FRAG_RBUF_H_

#include <inttypes.h>

#include "net/gnrc/netif/hdr.h"
#include "net/gnrc/pkt.h"
#include "timex.h"

#include "net/gnrc/sixlowpan/frag.h"
#ifdef __cplusplus

extern "C" {
#endif

#define RBUF_L2ADDR_MAX_LEN (8U)    /**< maximum length for link-layer addresses */
#define RBUF_SIZE           (4U)    /**< size of the reassembly buffer */
#define RBUF_TIMEOUT        (3U)    /**< timeout for reassembly in seconds */

/**
 * @brief   Fragment intervals to identify limits of fragments.
 *
 * @note    Fragments MUST NOT overlap and overlapping fragments are to be
 *          discarded
 *
 * @see <a href="https://tools.ietf.org/html/rfc4944#section-5.3">
 *          RFC 4944, section 5.3
 *      </a>
 *
 * @internal
 */
typedef struct rbuf_int {
    struct rbuf_int *next;  /**< next element in interval list */
    uint16_t start;         /**< start byte of interval */
    uint16_t end;           /**< end byte of interval */
} rbuf_int_t;

/**
 * @brief   An entry in the 6LoWPAN reassembly buffer.
 *
 * @details A receipient of a fragment SHALL use
 *
 * 1. the source address,
 * 2. the destination address,
 * 3. the datagram size (gnrc_pktsnip_t::size of rbuf_t::pkt), and
 * 4. the datagram tag
 *
 * to identify all fragments that belong to the given datagram.
 *
 * @see <a href="https://tools.ietf.org/html/rfc4944#section-5.3">
 *          RFC 4944, section 5.3
 *      </a>
 *
 * @internal
 */
typedef struct {
    rbuf_int_t *ints;                   /**< intervals of the fragment */
    gnrc_pktsnip_t *pkt;                /**< the reassembled packet in packet buffer */
    uint32_t arrival;                   /**< time in seconds of arrival of last
                                         *   received fragment */
    uint8_t src[RBUF_L2ADDR_MAX_LEN];   /**< source address */
    uint8_t dst[RBUF_L2ADDR_MAX_LEN];   /**< destination address */
    uint8_t src_len;                    /**< length of source address */
    uint8_t dst_len;                    /**< length of destination address */
    uint16_t tag;                       /**< the datagram's tag */
    uint16_t cur_size;                  /**< the datagram's current size */
} rbuf_t;

/**
 * @brief   Adds a new fragment to the reassembly buffer. If the packet is
 *          complete, dispatch the packet with the transmit information of
 *          the last fragment.
 *
 * @param[in] netif_hdr     The interface header of the fragment, with
 *                          gnrc_netif_hdr_t::if_pid and its source and
 *                          destination address set.
 * @param[in] frag          The fragment to add.
 * @param[in] frag_size     The fragment's size.
 * @param[in] offset        The fragment's offset.
 *
 * @internal
 */
void rbuf_add(gnrc_netif_hdr_t *netif_hdr, gnrc_pktsnip_t *frag,
              size_t frag_size, size_t offset);

#ifdef __cplusplus
}
#endif

#endif /* GNRC_SIXLOWPAN_FRAG_RBUF_H_ */
/** @} */
