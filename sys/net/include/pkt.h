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
 * @file
 * @brief   General definitions for network packets
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef __PKT_H_
#define __PKT_H_

#include <stdlib.h>
#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Definition of protocol families to determine the type of the packet
 *          or which protocols a network device (see @ref netdev) or protocol
 *          layer (see @ref netapi) can handle
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

    PKT_PROTO_ETHERNET      = 0x0002,   /**< Ethernet */
    PKT_PROTO_802154_BEACON = 0x0003,   /**< IEEE 802.15.4 beacon frame */
    PKT_PROTO_802154_DATA   = 0x0004,   /**< IEEE 802.15.4 data frame */
    PKT_PROTO_802154_ACK    = 0x0005,   /**< IEEE 802.15.4 acknowledgment frame */
    PKT_PROTO_802154_MACCMD = 0x0006,   /**< IEEE 802.15.4 MAC command frame */
    PKT_PROTO_BTLE          = 0x0007,   /**< Bluetooth Low-Energy */

    /**
     * @brief   CC110x frame format protocol
     *
     * @details Sends frames as defined by cc110x_packet_t.
     */
    PKT_PROTO_CC110X        = 0x0008,
    PKT_PROTO_6LOWPAN       = 0x0009,   /**< 6LoWPAN. */
    PKT_PROTO_IPV4          = 0x000a,   /**< IPv4. */
    PKT_PROTO_IPV6          = 0x000b,   /**< IPv6. */
    PKT_PROTO_UDP           = 0x000c,   /**< UDP. */
    PKT_PROTO_TCP           = 0x000d,   /**< TCP. */
    PKT_PROTO_CCNL          = 0x000e,   /**< CCN lite. */
} pkt_proto_t;

/**
 * @brief   Type to define payload size of a packet
 */
typedef uint16_t pktsize_t;

/**
 * @brief   Macro for pktsize_t printing formatter
 */
#define PRIpktsize  PRIu16

/**
 * @brief   Maximum value for packet size
 */
#define PKTSIZE_MAX (UINT16_MAX)

/**
 * @brief   Circular list type to store a number of protocol headers of
 *          unspecified type to work with @ref clist.h.
 *
 * @note    This type implements its own list implementation because of the way
 *          it is stored in the packet buffer.
 */
typedef struct __attribute__((packed)) pkt_hlist_t {    /* packed to be aligned
                                                         * correctly in static
                                                         * packet buffer */
    struct pkt_hlist_t *next;   /**< next element in list. */
    void *header_data;          /**< the data of the header */
    pktsize_t header_len;       /**< the length of the header in byte. */
    pkt_proto_t header_proto;   /**< protocol of the header. */
} pkt_hlist_t;

/**
 * @brief   Type to represent a network packet
 *
 * @note    This type has not an initializer on purpose. Please use @ref pktbuf
 *          as factory.
 */
typedef struct {
    pkt_hlist_t *headers;       /**< network protocol headers of the packet. */
    void *payload_data;         /**< payload of the packet. */
    pktsize_t payload_len;      /**< length of pkt_t::payload. */
} pkt_t;

/**
 * @brief   Initializes a packet with data from the @ref pktbuf
 *
 * @param[out] pkt          The packet to initialize.
 * @param[in] payload_data  Data to insert into initialized packet.
 *                          If @p payload_data is NULL @ref pktbuf_alloc() will
 *                          be used, otherwise @ref pktbuf_insert() to allocate
 *                          the data. If @p payload_data is already in @ref pktbuf
 *                          it will not be allocated again.
 * @param[in] payload_len   Length of @p payload_data
 *
 * @return Copy of @p pkt on success
 * @return NULL, if no space is left in the packet buffer or size was 0.
 */
pkt_t *pkt_init(pkt_t *pkt, void *payload_data, pktsize_t payload_len);

/**
 * @brief   Alias for `pkt_alloc(pkt, NULL, payload_len)`
 *
 * @param[out] pkt          The packet to initialize.
 *                          If @p payload_data is NULL @ref pktbuf_alloc() will
 *                          be used, otherwise @ref pktbuf_insert() to allocate
 *                          the data.
 * @param[in] payload_len   Length of @p payload_data
 *
 * @return Copy of @p pkt on success
 * @return NULL, if no space is left in the packet buffer or size was 0.
 */
static inline pkt_t *pkt_alloc(pkt_t *pkt, pktsize_t payload_len)
{
    return pkt_init(pkt, NULL, payload_len);
}

/**
 * @brief   Helper function to resize data part of @p pkt to another size
 *          of @p payload_len, using @ref pktbuf_realloc()
 *
 * @param[in,out] pkt       The packet which data you want to resize.
 * @param[in] payload_len   New length for @p pkt::payload_data
 *
 * @return Copy of @p pkt on success
 * @return NULL, if no space is left in the packet buffer or size was 0.
 *         @p pkt::payload_data will remain where it was.
 */
pkt_t *pkt_realloc(pkt_t *pkt, pktsize_t payload_len);

/**
 * @brief   Holds a packet in @ref pktbuf
 *
 * @see     @ref pktbuf_hold()
 *
 * @param[in] pkt   The packet to hold.
 */
void pkt_hold(pkt_t *pkt);

/**
 * @brief   Releases a packet in @ref pktbuf
 *
 * @see     @ref pktbuf_release()
 *
 * @param[in] pkt   The packet to release.
 */
void pkt_release(pkt_t *pkt);

/**
 * @brief Calculates total length of a list of headers.
 *
 * @param[in] list  list of headers.
 *
 * @note    The pkt_hlist_t type implements its own list implementation because
 *          of the way it is stored in the packet buffer.
 *
 * @return  length of the list of headers.
 */
pktsize_t pkt_hlist_len(pkt_hlist_t *list);

/**
 * @brief Advance the header list
 *
 * @param[in,out] list  The list to work upon.
 *
 * @return  The next element in @p list
 * @return  NULL if list reached its end
 */
static inline pkt_hlist_t *pkt_hlist_advance(pkt_hlist_t **list)
{
    if (list != NULL && *list != NULL) {
        *list = (*list)->next;
        return *list;
    }

    return NULL;
}

/**
 * @brief Adds a new header to a header list.
 *
 * @param[in,out] list  The list add the @p header to.
 * @param[in] header    The header to add to @p list.
 */
static inline void pkt_hlist_add(pkt_hlist_t **list, pkt_hlist_t *header)
{
    if (header == NULL || list == NULL) {
        return;
    }

    header->next = (*list);
    *list = header;
}

/**
 * @brief Removes and return first header from a header list.
 *
 * @param[in,out] list  The list remove the @p header from.
 *
 * @return The previously first header in @p list.
 * @return NULL if @p list is empty (aka `== NULL`)
 */
pkt_hlist_t *pkt_hlist_remove_first(pkt_hlist_t **list);

/**
 * @brief Removes a header from a header list.
 *
 * @param[in,out] list  The list remove the @p header from.
 * @param[in] header    The header to remove from @p list.
 */
void pkt_hlist_remove(pkt_hlist_t **list, pkt_hlist_t *header);

/**
 * @brief   Helper function to calculate the total length of the headers of
 *          @p pkt.
 *
 * @note    Wrapper function for @ref pkt_hlist_len()
 *
 * @param[in]   pkt   A network packet.
 *
 * @return  Length in number of bytes of all headers in pkt::headers.
 */
static inline pktsize_t pkt_total_header_len(const pkt_t *pkt)
{
    return pkt_hlist_len(pkt->headers);
}

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
 * @brief Adds a header to packet @p pkt and allocates its data in @ref pktbuf.
 *
 * @param[in,out] pkt       The packet to add the header to
 * @param[in] header_data   Data to insert into the added headr.
 *                          If @p header_data is NULL @ref pktbuf_alloc() will
 *                          be used, otherwise @ref pktbuf_insert() to allocate
 *                          the data. If @p header_data is already in @ref pktbuf
 *                          it will not be allocated again.
 * @param[in] header_len    Length of @p header_data
 * @param[in] header_proto  Protocol of the header.
 *
 * @return  -EFAULT, if @p header_data is NULL and DEVELHELP is defined.
 * @return  -ENOMEM, if no space is left in the packet buffer or @p header_len was 0.
 */
int pkt_add_header(pkt_t *pkt, void *header_data, pktsize_t header_len,
                   pkt_proto_t header_proto);

/**
 * @brief Removes @p header from packet @p pkt.
 *
 * @note    Wrapper function for @ref pkt_hlist_remove()
 *
 * @param[in,out] pkt   The packet to remove @p header from. May not be NULL.
 * @param[in] header    The header to remove from the list of headers of @p pkt.
 */
void pkt_remove_header(pkt_t *pkt, pkt_hlist_t *header);

/**
 * @brief Removes first (lowest layer) header from packet @p pkt and returns it.
 *
 * @note    Wrapper function for @ref pkt_hlist_remove_first()
 *
 * @param[in,out] pkt   The packet to remove @p header from.
 *
 * @return The previously first header of @p pkt.
 * @return NULL if  @p pkt is empty (aka `== NULL`)
 */
static inline pkt_hlist_t *pkt_remove_first_header(pkt_t *pkt)
{
    return (pkt == NULL) ? NULL : pkt_hlist_remove_first(&(pkt->headers));
}

#ifdef __cplusplus
}
#endif

#endif /* __PKT_H_ */
/** @} */
