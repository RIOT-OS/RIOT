/*
 * Copyright (C) 2014, 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    ng_pkt Packet
 * @brief       Network packet abstraction types
 * @ingroup     net
 * @{
 *
 * @file
 * @brief   General definitions for network packets
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef NG_PKT_H_
#define NG_PKT_H_

#include <inttypes.h>
#include <stdlib.h>

#include "net/ng_nettype.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Type to represent snips (or parts,
 *          snip == (packet header || packet payload)) of a network packet
 * @details The idea behind the packet snips is that they either can represent
 *          protocol-specific headers or payload. A packet can be comprised of
 *          *n* `pktsnip_t` elements, where the first element represents the
 *          header of the lowest available network layer and the *(n - 1)*st
 *          element represents the payload of the highest available layer.
 *          Use @ref sys_ut to operate on this.
 *
 *          Example:
 *
 *                                                                  buffer
 *              +---------------------------+                      +------+
 *              | size = 14                 | data +-------------->|      |
 *              | type = PKT_PROTO_ETHERNET |------+               +------+
 *              +---------------------------+         +----------->|      |
 *                    | next                          |            |      |
 *                    v                               |            |      |
 *              +---------------------------+         |            +------+
 *              | size = 40                 | data    |  +-------->|      |
 *              | type = PKT_PROTO_IPV6     |---------+  |         +------+
 *              +---------------------------+            |  +----->|      |
 *                    | next                             |  |      +------+
 *                    v                                  |  |  +-->|      |
 *              +---------------------------+            |  |  |   |      |
 *              | size = 8                  | data       |  |  |   .      .
 *              | type = PKT_PROTO_UDP      |------------+  |  |   .      .
 *              +---------------------------+               |  |
 *                    | next                                |  |
 *                    v                                     |  |
 *              +---------------------------+               |  |
 *              | size = 5                  | data          |  |
 *              | type = PKT_PROTO_COAP     |---------------+  |
 *              +---------------------------+                  |
 *                    | next                                   |
 *                    v                                        |
 *              +---------------------------+                  |
 *              | size = 54                 | data             |
 *              | type = PKT_PROTO_UNKNOWN  |------------------+
 *              +---------------------------+
 *
 *
 * @note    This type implements its own list implementation because of the way
 *          it is stored in the packet buffer.
 * @note    This type has no initializer on purpose. Please use @ref pktbuf
 *          as factory.
 */
typedef struct __attribute__((packed)) ng_pktsnip {   /* packed to be aligned
                                                       * correctly in the static
                                                       * packet buffer */
    struct ng_pktsnip *next;        /**< next snip in the packet */
    void *data;                     /**< pointer to the data of the snip */
    size_t size;                    /**< the length of the snip in byte */
    ng_nettype_t type;              /**< protocol of the packet snip */
} ng_pktsnip_t;

/**
 * @brief Calculates length of a packet in byte.
 *
 * @param[in] pkt  list of packet snips.
 *
 * @return  length of the list of headers.
 */
static inline size_t ng_pkt_len(ng_pktsnip_t *pkt)
{
    size_t len = 0;

    while (pkt) {
        len += pkt->size;
        pkt = pkt->next;
    }

    return len;
}

#ifdef __cplusplus
}
#endif

#endif /* NG_PKT_H_ */
/** @} */
