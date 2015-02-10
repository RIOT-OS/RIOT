/*
 * Copyright (C) 2014 Martine Lenders <mlenders@inf.fu-berlin.de>
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
#include "netmod.h"

#ifdef __cplusplus
extern "C" {
#endif

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
    netmod_t header_proto;      /**< protocol of the header. */
} pkt_hlist_t;

/**
 * @brief   Type to represent a network packet
 *
 * @note    This type has not an initializer on purpose. Please use @ref pktbuf
 *          as factory.
 */
typedef struct __attribute__((packed)) {    /* packed to be aligned correctly
                                             * in static packet buffer */
    pkt_hlist_t *headers;       /**< network protocol headers of the packet. */
    void *payload_data;              /**< payload of the packet. */
    pktsize_t payload_len;      /**< length of pkt_t::payload. */
    netmod_t payload_proto;  /**< protocol of pkt_t::payload, if any */
} pkt_t;

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
 * @brief Adds @p header to packet @p pkt.
 *
 * @note    Wrapper function for @ref pkt_hlist_add()
 *
 * @param[in,out] pkt   The packet to add @p header to
 * @param[in] header    The header to add to the list of headers of @p pkt.
 */
static inline void pkt_add_header(pkt_t *pkt, pkt_hlist_t *header)
{
    if (pkt == NULL) {
        return;
    }

    pkt_hlist_add(&(pkt->headers), header);
}

/**
 * @brief Removes @p header from packet @p pkt.
 *
 * @note    Wrapper function for @ref pkt_hlist_remove()
 *
 * @param[in,out] pkt   The packet to remove @p header from. May not be NULL.
 * @param[in] header    The header to remove from the list of headers of @p pkt.
 */
static inline void pkt_remove_header(pkt_t *pkt, pkt_hlist_t *header)
{
    if (pkt == NULL) {
        return;
    }

    pkt_hlist_remove(&(pkt->headers), header);
}

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
    if (pkt == NULL) {
        return NULL;
    }

    return pkt_hlist_remove_first(&(pkt->headers));
}

#ifdef __cplusplus
}
#endif

#endif /* __PKT_H_ */
/** @} */
