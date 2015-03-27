/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_ng_netif_hdr Generic network interface header
 * @ingroup     net_ng_netif
 * @{
 *
 * @file
 * @brief       Generic network interface header
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#ifndef NETIF_HDR_H_
#define NETIF_HDR_H_

#include <string.h>
#include <stdint.h>

#include "kernel.h"
#include "net/ng_pkt.h"
#include "net/ng_pktbuf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @{
 * @name    Flags for the ng_netif_hdr_t
 */
/**
 * @brief   Send packet broadcast.
 *
 * @details Packets with this flag set must be send broadcast.
 *          ng_netif_hdr_t::dst_l2addr_len and any appended destination
 *          address must be ignored.
 *          If the link layer does not support broadcast the packet must be
 *          dropped silently.
 */
#define NG_NETIF_HDR_FLAGS_BROADCAST    (0x80)

/**
 * @brief   Send packet multicast.
 *
 * @details Packets with this flag set must be send multicast.
 *          ng_netif_hdr_t::dst_l2addr_len and any appended destination
 *          address must be ignored.
 *          The context for the multicast address must be derived from the
 *          network layer destination address.
 *          If the link layer does not support multicast it should interpret
 *          this flag the same way it does @ref NG_NETIF_HDR_FLAGS_BROADCAST.
 */
#define NG_NETIF_HDR_FLAGS_MULTICAST    (0x40)
/**
 * @}
 */

/**
 * @brief   Generic network interface header
 *
 * The link layer addresses included in this header are put in memory directly
 * following this struct.
 */
typedef struct __attribute__((packed)) {
    uint8_t src_l2addr_len;     /**< length of l2 source address in byte */
    uint8_t dst_l2addr_len;     /**< length of l2 destination address in byte */
    kernel_pid_t if_pid;        /**< PID of network interface */
    uint8_t flags;              /**< flags as defined above */
    uint8_t rssi;               /**< rssi of received packet (optional) */
    uint8_t lqi;                /**< lqi of received packet (optional) */
} ng_netif_hdr_t;

/**
 * @brief   Initialize the given generic network interface header
 *
 * @param[in] hdr               header to initialize
 * @param[in] src_l2addr_len    link layer source address length
 * @param[in] dst_l2addr_len    link layer destination address length
 */
static inline void ng_netif_hdr_init(ng_netif_hdr_t *hdr, uint8_t src_l2addr_len,
                                     uint8_t dst_l2addr_len)
{
    hdr->src_l2addr_len = src_l2addr_len;
    hdr->dst_l2addr_len = dst_l2addr_len;
    hdr->if_pid = KERNEL_PID_UNDEF;
    hdr->rssi = 0;
    hdr->lqi = 0;
    hdr->flags = 0;
}

/**
 * @brief   Get the size of the given generic network interface header
 *
 * @param[in] hdr           header to get the size of
 *
 * @return                  the size of the given header, including link layer
 *                          addresses
 */
static inline size_t ng_netif_hdr_sizeof(ng_netif_hdr_t *hdr)
{
    return sizeof(ng_netif_hdr_t) + hdr->src_l2addr_len + hdr->dst_l2addr_len;
}

/**
 * @brief   Get the source address from the given header
 *
 * @param[in] hdr           header to read from
 *
 * @return                  pointer to source address on success
 * @return                  NULL on error
 */
static inline uint8_t *ng_netif_hdr_get_src_addr(ng_netif_hdr_t *hdr)
{
    return ((uint8_t *)(hdr + 1));
}

/**
 * @brief   Set the source address in the given header
 *
 * @param[in] hdr           header to write to
 * @param[in] addr          new source address
 * @param[in] addr_len      *addr* length
 */
static inline void ng_netif_hdr_set_src_addr(ng_netif_hdr_t *hdr, uint8_t *addr,
        uint8_t addr_len)
{
    if (addr_len != hdr->src_l2addr_len) {
        return;
    }

    memcpy(((uint8_t *)(hdr + 1)), addr, addr_len);
}


/**
 * @brief   Get the destination address from the given header
 *
 * @param[in] hdr           header to read from
 *
 * @return                  pointer to destination address on success
 * @return                  NULL on error
 */
static inline uint8_t *ng_netif_hdr_get_dst_addr(ng_netif_hdr_t *hdr)
{
    return (((uint8_t *)(hdr + 1)) + hdr->src_l2addr_len);
}

/**
 * @brief   Set the destination address in the given header
 *
 * @param[in] hdr           header to write to
 * @param[in] addr          new destination address
 * @param[in] addr_len      *addr* length
 */
static inline void ng_netif_hdr_set_dst_addr(ng_netif_hdr_t *hdr, uint8_t *addr,
        uint8_t addr_len)
{
    if (addr_len != hdr->dst_l2addr_len) {
        return;
    }

    memcpy(((uint8_t *)(hdr + 1)) + hdr->src_l2addr_len, addr, addr_len);
}

/**
 * @brief   Builds a generic network interface header for sending and
 *          adds it to the packet buffer.
 *
 * @param[in] src       Source address for the header. Can be NULL if not
 *                      known or required.
 * @param[in] src_len   Length of @p src. Can be 0 if not known or required.
 * @param[in] dst       Destination address for the header. Can be NULL if not
 *                      known or required.
 * @param[in] dst_len   Length of @p dst. Can be 0 if not known or required.
 *
 * @return  The generic network layer header on success.
 * @return  NULL on error.
 */
static inline ng_pktsnip_t *ng_netif_hdr_build(uint8_t *src, uint8_t src_len,
                                               uint8_t *dst, uint8_t dst_len)
{
    ng_pktsnip_t *pkt = ng_pktbuf_add(NULL, NULL,
                                      sizeof(ng_netif_hdr_t) + src_len + dst_len,
                                      NG_NETTYPE_NETIF);

    if (pkt == NULL) {
        return NULL;
    }

    ng_netif_hdr_init(pkt->data, src_len, dst_len);

    if (src != NULL && src_len > 0) {
        ng_netif_hdr_set_src_addr(pkt->data, src, src_len);
    }

    if (dst != NULL && dst_len > 0) {
        ng_netif_hdr_set_dst_addr(pkt->data, dst, dst_len);
    }

    return pkt;
}

#ifdef __cplusplus
}
#endif

#endif /* NETIF_HDR_H_ */
/** @} */
