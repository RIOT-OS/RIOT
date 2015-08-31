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
#ifndef GNRC_NDP_NODE_H_
#define GNRC_NDP_NODE_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Next hop determination for a destination address that is not on-link.
 *
 * @see <a href="https://tools.ietf.org/html/rfc4861#section-3">
 * RFC 4861, section 3
 * </a>
 *
 * @param[out] next_hop_ip  Will be filled with IPv6 address of the next hop
 *                          or NULL if no next hop could be determined.
 * @param[in,out] iface     Will be filled with the interface id that has to be
 *                          used for reaching the next hop.
 * @param[in] dst           The destination IPv6 address.
 *
 * @return true, if a next hop could be determined.
 * @return false otherwise
 */
bool gnrc_ndp_node_next_hop_ipv6_addr(ipv6_addr_t *next_hop_ip,
                                      kernel_pid_t *iface, ipv6_addr_t *dst);

/**
 * @brief   Get link-layer address a given IPv6 address.
 *
 * @param[out] l2addr           The link-layer of @p dst.
 * @param[out] l2addr_len       Length of @p l2addr.
 * @param[in] iface             The interface to search the next hop on.
 *                              May be @ref KERNEL_PID_UNDEF if not specified.
 * @param[in] dst               The IPv6 address to search the link-layer
 *                              address hop for.
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

#endif /* GNRC_NDP_NODE_H_ */
/** @} */
