/*
 * Copyright (C) 2014 Martin Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    pkt Packet
 * @ingroup     net
 * @{
 *
 * @file    pkt.h
 * @brief   General definitions for network packets
 *
 * @note    This file resides in the `sys` module's include path since it is
 *          needed for network device drivers and the `sys/net` module's include
 *          path is not always included (and netdev does not necessarily needs
 *          to a compiled `pkt`).
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef __PKT_H_
#define __PKT_H_

#include <inttypes.h>

#include "clist.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Definition of protocol families to determine the type of the packet or which protocols a network device (see @ref netdev) or protocol layer (see @ref netapi) can handle
 *
 * @note    XXX: The concrete definition of the values is necessary to work
 *          with super-flexible devices as e.g. @ref native_net. It was also
 *          decided not to use ethertype since protocols not supplied by it
 *          might be supported
 */
typedef enum {
    PKT_PROTO_UNKNOWN       = 0x0000,   /**< Type was not specified */

    /**
     * @brief   Radio frame protocol
     *
     * @details Sends frames as defined by radio_packet_t.
     */
    PKT_PROTO_RADIO         = 0x0001,

    /**
     * @brief   IEEE 802.15.4
     *
     * @details Sends frames as defined by ieee802154_frame_t
     */
    PKT_PROTO_802154        = 0x0002,
    PKT_PROTO_6LOWPAN       = 0x0003,   /**< 6LoWPAN. */
    PKT_PROTO_IPV6          = 0x0004,   /**< IPv6. */
    PKT_PROTO_UDP           = 0x0005,   /**< UDP. */
    PKT_PROTO_TCP           = 0x0006,   /**< TCP. */
    PKT_PROTO_CCNL          = 0x0007,   /**< CCN lite. */

    /**
     * @brief   CC110x frame format protocol
     *
     * @detail  Sends frames as defined by cc110x_packet_t.
     */
    PKT_PROTO_CC110X        = 0x0008,
} pkt_proto_t;

/**
 * @brief   Type to define payload size of a packet
 */
typedef uint16_t pktsize_t;

/**
 * @brief   Macro for pktsize_t printing formatter
 */
#define PRIpktsize  (PRIu16)

/**
 * @brief   Maximum value for packet size
 */
#define PKTSIZE_MAX (UINT16_MAX)

/**
 * @brief   Circular list type to store a number of protocol headers of
 *          unspecified type to work with @ref clist.h.
 *
 * @extends clist_node_t
 */
typedef struct __attribute__((packed)) pkt_hlist_t {
    struct pkt_hlist_t *next;   /**< next element in list. */
    struct pkt_hlist_t *prev;   /**< previous element in list. */
    pkt_proto_t protocol;       /**< protocol of the header. */
    void *header_data;          /**< the data of the header */
    uint8_t header_len;         /**< the length of the header in byte. */
} pkt_hlist_t;

/**
 * @brief   Type to represent a network packet
 *
 * @note    This type has not an initializer on purpose. Please use @ref pktbuf
 *          as factory.
 */
typedef struct __attribute__((packed)) {
    pkt_hlist_t *headers;   /**< network protocol headers of the packet. */
    void *payload;          /**< payload of the packet. */
    pktsize_t payload_len;  /**< length of pkt_t::payload. */
} pkt_t;

/**
 * @brief   Helper function to calculate the total length of the headers of *pkt*.
 *_
 * @param[in]   pkt   A network packet.
 *
 * @return  Length in number of bytes of all headers in pkt::headers.
 */
pktsize_t pkt_total_header_len(const pkt_t *pkt);

/**
 * @brief   Calculate total length of the packet.
 *
 * @return  Total length of the packet in number of bytes
 */
static inline pktsize_t pkt_total_len(const pkt_t *pkt)
{
    return pkt_total_header_len(pkt) + pkt->payload_len;
}

/**
 * @brief Advance the header list
 *
 * @see clist_advance
 *
 * @param[in,out] list  The list to work upon.
 */
static inline void pkt_hlist_advance(pkt_hlist_t **list)
{
    if (list != NULL && *list != NULL) {
        clist_advance((clist_node_t **)list);
    }
}

/**
 * @brief Add *header* to the start of headers of *pkt*
 *
 * @param[in,out] pkt   The packet to add *header* to. May not be NULL.
 * @param[in] header    The header to add to the list of headers of *pkt*.
 *                      May not be NULL.
 */
static inline void pkt_add_header(pkt_t *pkt, pkt_hlist_t *header)
{
    clist_add((clist_node_t **)(&(pkt->headers)), (clist_node_t *)header);
    pkt->headers = header;
}

/**
 * @brief Removes @p header from packet @p pkt
 *
 * @see clist_remove
 *
 * @param[in,out] pkt   The packet to remove *header* from. May not be NULL.
 * @param[in] header    The header to remove from the list of headers of *pkt*.
 *                      May not be NULL.
 */
static inline void pkt_remove_header(pkt_t *pkt, pkt_hlist_t *header)
{
    clist_remove((clist_node_t **)(&(pkt->headers)), (clist_node_t *)header);
}

/**
 * @brief Removes first (lowest layer) header from packet @p pkt and returns it.
 *
 * @see clist_removee
 *
 * @param[in,out] pkt   The packet to remove *header* from. May not be NULL.
 */
static inline pkt_hlist_t *pkt_remove_first_header(pkt_t *pkt)
{
    pkt_hlist_t *first = pkt->headers;
    pkt_remove_header(pkt, pkt->headers);
    return first;
}

#ifdef __cplusplus
}
#endif

#endif /* __PKT_H_ */
/** @} */
