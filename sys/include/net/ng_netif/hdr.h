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
 */

#ifndef NETIF_HDR_H_
#define NETIF_HDR_H_

#include <string.h>
#include <stdint.h>

#include "kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif

#endif /* NETIF_HDR_H_ */
/** @} */
