/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_ndp_node Neighbor discovery for pure IPv6 nodes
 * @ingroup     net_gnrc_ndp
 * @brief       Used for pure IPv6 nodes (without 6LoWPAN).
 * @{
 *
 * @file
 * @brief   IPv6-node neighbor discovery definitions
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef GNRC_NDP_NODE_H
#define GNRC_NDP_NODE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Get link-layer address and interface for next hop to destination
 *          IPv6 address.
 *
 * @param[out] l2addr           The link-layer for the next hop to @p dst.
 * @param[out] l2addr_len       Length of @p l2addr.
 * @param[in] iface             The interface to search the next hop on.
 *                              May be @ref KERNEL_PID_UNDEF if not specified.
 * @param[in] dst               An IPv6 address to search the next hop for.
 * @param[in] pkt               Packet to send to @p dst. Leave NULL if you
 *                              just want to get the addresses.
 *
 * @return  The PID of the interface, on success.
 * @return  -EHOSTUNREACH, if @p dst is not reachable.
 * @return  -ENOBUFS, if @p l2addr_len was smaller than the resulting @p l2addr
 *          would be long.
 */
kernel_pid_t gnrc_ndp_node_next_hop_l2addr(uint8_t *l2addr, uint8_t *l2addr_len,
                                           kernel_pid_t iface, ipv6_addr_t *dst,
                                           gnrc_pktsnip_t *pkt);

#ifdef __cplusplus
}
#endif

#endif /* GNRC_NDP_NODE_H */
/** @} */
