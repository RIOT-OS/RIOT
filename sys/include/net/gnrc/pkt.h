/*
 * Copyright (C) 2014, 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *               2015       Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_pkt Packet
 * @ingroup     net_gnrc_pktbuf
 * @brief       Network packet abstraction type and helper functions
 * @{
 *
 * @file
 * @brief   General definitions for network packets and their helper functions
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 * @author  Hauke Petersen <hauke.petersen@fu-berlin.de>
 */
#ifndef NET_GNRC_PKT_H
#define NET_GNRC_PKT_H

#include <inttypes.h>
#include <stdlib.h>

#include "sched.h"
#include "net/gnrc/nettype.h"
#include "list.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Type to represent parts (either headers or payload) of a packet,
 *          called snips.
 * @details The idea behind the packet snips is that they either can represent
 *          protocol-specific headers or payload. A packet can be comprised of
 *          multiple pktsnip_t elements.
 *
 *          Example:
 *
 *                                                                  buffer
 *              +---------------------------+                      +------+
 *              | size = 14                 | data +-------------->|      |
 *              | type = NETTYPE_ETHERNET   |------+               +------+
 *              +---------------------------+                      .      .
 *                    | next                                       .      .
 *                    v                                            +------+
 *              +---------------------------+         +----------->|      |
 *              | size = 40                 | data    |            |      |
 *              | type = NETTYPE_IPV6       |---------+            +------+
 *              +---------------------------+                      .      .
 *                    | next                                       .      .
 *                    v                                            +------+
 *              +---------------------------+            +-------->|      |
 *              | size = 8                  | data       |         +------+
 *              | type = NETTYPE_UDP        |------------+         .      .
 *              +---------------------------+                      .      .
 *                    | next                                       +------+
 *                    v                                     +----->|      |
 *              +---------------------------+               |      |      |
 *              | size = 59                 | data          |      .      .
 *              | type = NETTYPE_UNDEF      |---------------+      .      .
 *              +---------------------------+                      .      .
 *
 * To keep data duplication as low as possible the order of the snips
 * in a packet will be reversed depending on if you send the packet or if
 * you received it. For sending the order is from (in the network stack) lowest
 * protocol snip to the highest, for receiving the order is from highest
 * snip to the lowest. This way, if a layer needs to duplicate the packet
 * a tree is created rather than a duplication of the whole package.
 *
 * A very extreme example for this (we only expect one or two duplications at
 * maximum per package) can be seen here:
 *
 *      Sending                          Receiving
 *      =======                          =========
 *
 *      * Payload                        * L2 header
 *      ^                                ^
 *      |                                |
 *      |\                               |\
 *      | * L4 header 1                  | * L2.5 header 1
 *      | * L3 header 1                  | * L3 header 1
 *      | * netif header 1               | * L4 header 1
 *      * L4 header 2                    | * Payload 1
 *      ^                                * L3 header 2
 *      |                                ^
 *      |\                               |
 *      | * L3 header 2                  |\
 *      | * L2 header 2                  | * L4 header 2
 *      * L2 header 3                    | * Payload 2
 *      |\                               * Payload 3
 *      | * L2 header 3
 *      * L2 header 4
 *
 * The first three fields (next, data, size) match iolist_t (named iol_next,
 * iol_base and iol_len there).  That means that any pktsnip can be casted to
 * iolist_t for direct passing to e.g., netdev send() functions.
 *
 * @note    This type has no initializer on purpose. Please use @ref net_gnrc_pktbuf
 *          as factory.
 */
/* packed to be aligned correctly in the static packet buffer */
typedef struct gnrc_pktsnip {
    /* the first three fields *MUST* match iolist_t! */
    struct gnrc_pktsnip *next;      /**< next snip in the packet */
    void *data;                     /**< pointer to the data of the snip */
    size_t size;                    /**< the length of the snip in byte */
    /**
     * @brief   Counter of threads currently having control over this packet.
     *
     * @internal
     */
    unsigned int users;
    gnrc_nettype_t type;            /**< protocol of the packet snip */
#ifdef MODULE_GNRC_NETERR
    kernel_pid_t err_sub;           /**< subscriber to errors related to this
                                     *   packet snip */
#endif
} gnrc_pktsnip_t;

/**
 * @brief   Returns the snip before a given snip in a packet
 *
 * @param[in] pkt   A packet.
 * @param[in] snip  The snip for which the predecessor in @p pkt is searched for.
 *
 * @return  The snip before @p snip in @p pkt if @p snip is in @p pkt.
 * @return  `NULL`, if @p snip is not in @p pkt.
 */
static inline gnrc_pktsnip_t *gnrc_pkt_prev_snip(gnrc_pktsnip_t *pkt,
                                                 gnrc_pktsnip_t *snip)
{
    while ((pkt != NULL) && (pkt->next != snip)) {
        pkt = pkt->next;
    }
    return pkt;
}

/**
 * @brief Calculates length of a packet in byte.
 *
 * @param[in] pkt  list of packet snips.
 *
 * @return  length of the list of headers.
 */
static inline size_t gnrc_pkt_len(const gnrc_pktsnip_t *pkt)
{
    size_t len = 0;

    while (pkt != NULL) {
        len += pkt->size;
        pkt = pkt->next;
    }

    return len;
}

/**
 * @brief   Appends a snip to a packet.
 *
 * @param[in] pkt   A packet.
 * @param[in] snip  A snip.
 *
 * @return  The new head of @p pkt.
 */
static inline gnrc_pktsnip_t *gnrc_pkt_append(gnrc_pktsnip_t *pkt,
                                              gnrc_pktsnip_t *snip)
{
    /* find last snip in pkt */
    gnrc_pktsnip_t *last = gnrc_pkt_prev_snip(pkt, NULL);

    if (last != NULL) {
        last->next = snip;
    }
    else {
        /* last == NULL means snip */
        pkt = snip;
    }
    return pkt;
}

/**
 * @brief   Prepends a snip to a packet.
 *
 * @param[in] pkt   A packet.
 * @param[in] snip  A snip.
 *
 * @return  The new head of @p pkt.
 */
static inline gnrc_pktsnip_t *gnrc_pkt_prepend(gnrc_pktsnip_t *pkt,
                                               gnrc_pktsnip_t *snip)
{
    snip->next = pkt;
    return snip;
}

/**
 * @brief   Deletes a snip from a packet.
 *
 * @param[in] pkt   A packet.
 * @param[in] snip  A snip.
 *
 * @return  The new head of @p pkt.
 */
static inline gnrc_pktsnip_t *gnrc_pkt_delete(gnrc_pktsnip_t *pkt,
                                              gnrc_pktsnip_t *snip)
{
    /* Removing head is a no-op. The new head is the next in the list. */
    if (pkt == snip) {
        return pkt->next;
    }

    /* Removing nothing is a no-op, the new head is the old one */
    if (snip == NULL) {
        return pkt;
    }

    /* Iterate over the list and remove the given snip from it, if found.
     * The new head is the old head. */
    for (gnrc_pktsnip_t *i = pkt; i != NULL; i = i->next) {
        if (i->next == snip) {
            i->next = snip->next;
            return pkt;
        }
    }

    return pkt;
}

/**
 * @brief Calculates length of a packet in byte up to (including) a snip with the given type.
 *
 * @param[in] pkt  list of packet snips.
 * @param[in] type type of snip to stop calculation.
 *
 * @return  length of the list of headers.
 */
static inline size_t gnrc_pkt_len_upto(const gnrc_pktsnip_t *pkt, gnrc_nettype_t type)
{
    size_t len = 0;

    while (pkt != NULL) {
        len += pkt->size;

        if (pkt->type == type) {
            break;
        }

        pkt = pkt->next;
    }

    return len;
}

/**
 * @brief Count the numbers of snips in the given packet
 *
 * @param[in] pkt   first snip in the packet
 *
 * @return  number of snips in the given packet
 */
static inline size_t gnrc_pkt_count(const gnrc_pktsnip_t *pkt)
{
    size_t count = 0;

    while (pkt != NULL) {
        ++count;
        pkt = pkt->next;
    }

    return count;
}

/**
 * @brief   Searches the packet for a packet snip of a specific type
 *
 * @param[in] pkt   list of packet snips
 * @param[in] type  the type to search for
 *
 * @return  the packet snip in @p pkt with @ref gnrc_nettype_t @p type
 * @return  NULL, if none of the snips in @p pkt is of @p type
 */
gnrc_pktsnip_t *gnrc_pktsnip_search_type(gnrc_pktsnip_t *pkt,
                                         gnrc_nettype_t type);

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_PKT_H */
/** @} */
