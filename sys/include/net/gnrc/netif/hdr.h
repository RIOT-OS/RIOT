/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_netif_hdr Generic network interface header
 * @ingroup     net_gnrc_netif
 * @{
 *
 * @file
 * @brief       Generic network interface header
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#ifndef NET_GNRC_NETIF_HDR_H
#define NET_GNRC_NETIF_HDR_H

#include <errno.h>
#include <string.h>
#include <stdint.h>

#include "net/gnrc/netif/internal.h"
#include "net/gnrc/pkt.h"
#include "net/gnrc/pktbuf.h"
#include "net/gnrc/netif.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Maximum length of the l2 addresses of the generic interface header
 *          in bytes.
 */
#define GNRC_NETIF_HDR_L2ADDR_MAX_LEN   (8)

/**
 * @brief   Maximum length of the string representatiom of l2 addresses of the
 *          generic interface header in bytes.
 */
#define GNRC_NETIF_HDR_L2ADDR_PRINT_LEN (GNRC_NETIF_HDR_L2ADDR_MAX_LEN * 3)

/**
 * @{
 * @name    Flags for the gnrc_netif_hdr_t
 */
/**
 * @brief   Send packet broadcast.
 *
 * @details Packets with this flag set must be send broadcast.
 *          gnrc_netif_hdr_t::dst_l2addr_len and any appended destination
 *          address must be ignored.
 *          If the link layer does not support broadcast the packet must be
 *          dropped silently.
 */
#define GNRC_NETIF_HDR_FLAGS_BROADCAST  (0x80)

/**
 * @brief   Send packet multicast.
 *
 * @details Packets with this flag set must be send multicast.
 *          gnrc_netif_hdr_t::dst_l2addr_len and any appended destination
 *          address must be ignored.
 *          The context for the multicast address must be derived from the
 *          network layer destination address.
 *          If the link layer does not support multicast it should interpret
 *          this flag the same way it does @ref GNRC_NETIF_HDR_FLAGS_BROADCAST.
 */
#define GNRC_NETIF_HDR_FLAGS_MULTICAST  (0x40)

/**
 * @brief   More data will follow
 *
 * @details This flag signals that this packet is part of a burst of packets.
 *          The link layer implementation can choose to translate this flag into
 *          frame header bits to tell the remote node that more traffic will
 *          follow shortly. The most direct use case for this flag is to set it
 *          for fragmented packets in duty cycled networks to tell the remote
 *          node to keep its radio turned on after receiving the first fragment.
 *
 * @see     The corresponding bit in the IEEE 802.15.4 frame control field,
 *          @ref IEEE802154_FCF_FRAME_PEND
 */
#define GNRC_NETIF_HDR_FLAGS_MORE_DATA  (0x10)
/**
 * @}
 */

/**
 * @brief   Generic network interface header
 *
 * The link layer addresses included in this header are put in memory directly
 * following this struct.
 */
typedef struct {
    uint8_t src_l2addr_len;     /**< length of l2 source address in byte */
    uint8_t dst_l2addr_len;     /**< length of l2 destination address in byte */
    kernel_pid_t if_pid;        /**< PID of network interface */
    uint8_t flags;              /**< flags as defined above */
    uint8_t lqi;                /**< lqi of received packet (optional) */
    int16_t rssi;               /**< rssi of received packet in dBm (optional) */
} gnrc_netif_hdr_t;

/**
 * @brief   Initialize the given generic network interface header
 *
 * @param[in] hdr               header to initialize
 * @param[in] src_l2addr_len    link layer source address length
 * @param[in] dst_l2addr_len    link layer destination address length
 */
static inline void gnrc_netif_hdr_init(gnrc_netif_hdr_t *hdr, uint8_t src_l2addr_len,
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
static inline size_t gnrc_netif_hdr_sizeof(const gnrc_netif_hdr_t *hdr)
{
    return sizeof(gnrc_netif_hdr_t) + hdr->src_l2addr_len + hdr->dst_l2addr_len;
}

/**
 * @brief   Get the source address from the given header
 *
 * @param[in] hdr           header to read from
 *
 * @return                  pointer to source address on success
 * @return                  NULL on error
 */
static inline uint8_t *gnrc_netif_hdr_get_src_addr(const gnrc_netif_hdr_t *hdr)
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
static inline void gnrc_netif_hdr_set_src_addr(gnrc_netif_hdr_t *hdr,
                                               const uint8_t *addr,
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
static inline uint8_t *gnrc_netif_hdr_get_dst_addr(const gnrc_netif_hdr_t *hdr)
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
static inline void gnrc_netif_hdr_set_dst_addr(gnrc_netif_hdr_t *hdr,
                                               const uint8_t *addr,
                                               uint8_t addr_len)
{
    if (addr_len != hdr->dst_l2addr_len) {
        return;
    }

    memcpy(((uint8_t *)(hdr + 1)) + hdr->src_l2addr_len, addr, addr_len);
}

#if defined(MODULE_GNRC_IPV6) || defined(DOXYGEN)
/**
 * @brief   Converts the source address of a given @ref net_gnrc_netif_hdr to
 *          an IPv6 IID
 *
 * @note    @p netif is intentionally required to be provided so that the caller
 *          needs to retrieve it from gnrc_netif_hdr_t::if_pid of @p hdr only
 *          once instead of this function retrieving it at every call.
 *
 * @pre `netif->pid == hdr->if_pid`
 *
 * @param[in] netif A network interface. gnrc_netif_t::pid must be equal to
 *                  gnrc_netif_hdr_t::if_pid of @p hdr.
 * @param[in] hdr   Header to convert source address from.
 * @param[out] iid  The IID based on gnrc_netif_t::device_type.
 *
 * @return  same as gnrc_netif_ipv6_iid_from_addr().
 */
static inline int gnrc_netif_hdr_ipv6_iid_from_src(const gnrc_netif_t *netif,
                                                   const gnrc_netif_hdr_t *hdr,
                                                   eui64_t *iid)
{
    return gnrc_netif_ipv6_iid_from_addr(netif,
                                         gnrc_netif_hdr_get_src_addr(hdr),
                                         hdr->src_l2addr_len,
                                         iid);
}

/**
 * @brief   Converts the destination address of a given @ref net_gnrc_netif_hdr
 *          to an IPv6 IID
 *
 * @note    @p netif is intentionally required to be provided so that the caller
 *          needs to retrieve it from gnrc_netif_hdr_t::if_pid of @p hdr only
 *          once instead of this function retrieving it at every call.
 *
 * @pre `netif->pid == hdr->if_pid`
 *
 * @param[in] netif A network interface. gnrc_netif_t::pid must be equal to
 *                  gnrc_netif_hdr_t::if_pid of @p hdr.
 * @param[in] hdr   Header to convert destination address from.
 * @param[out] iid  The IID based on gnrc_netif_t::device_type.
 *
 * @return  same as gnrc_netif_ipv6_iid_from_addr().
 */
static inline int gnrc_netif_hdr_ipv6_iid_from_dst(const gnrc_netif_t *netif,
                                                   const gnrc_netif_hdr_t *hdr,
                                                   eui64_t *iid)
{
    return gnrc_netif_ipv6_iid_from_addr(netif,
                                         gnrc_netif_hdr_get_dst_addr(hdr),
                                         hdr->dst_l2addr_len,
                                         iid);
}
#else   /* defined(MODULE_GNRC_IPV6) || defined(DOXYGEN) */
#define gnrc_netif_hdr_ipv6_iid_from_src(netif, hdr, iid)   (-ENOTSUP);
#define gnrc_netif_hdr_ipv6_iid_from_dst(netif, hdr, iid)   (-ENOTSUP);
#endif  /* defined(MODULE_GNRC_IPV6) || defined(DOXYGEN) */

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
gnrc_pktsnip_t *gnrc_netif_hdr_build(const uint8_t *src, uint8_t src_len,
                                     const uint8_t *dst, uint8_t dst_len);

/**
 * @brief   Convenience function to get the corresponding interface struct for
 *          a given interface header
 *
 * @pre `hdr != NULL`
 *
 * @param[in] hdr   Header to read interface from.
 *
 * @return  The @ref gnrc_netif_t representation of the interface on success
 * @return  NULL, on error.
 */
static inline gnrc_netif_t *gnrc_netif_hdr_get_netif(const gnrc_netif_hdr_t *hdr)
{
    assert(hdr != NULL);
    return gnrc_netif_get_by_pid(hdr->if_pid);
}

/**
 * @brief   Convenience function to set the interface of an interface header,
 *          given the network interface
 *
 * @param[out] hdr  Header to set the interface for.
 * @param[in] netif Network interface to set for @p hdr.
 */
static inline void gnrc_netif_hdr_set_netif(gnrc_netif_hdr_t *hdr,
                                            const gnrc_netif_t *netif)
{
    hdr->if_pid = (netif != NULL) ? netif->pid : KERNEL_PID_UNDEF;
}

/**
 * @brief   Outputs a generic interface header to stdout.
 *
 * @param[in] hdr   A generic interface header.
 */
void gnrc_netif_hdr_print(gnrc_netif_hdr_t *hdr);

/**
 * @brief   Fetch the netif header flags of a gnrc packet
 *
 * @param[in]   pkt     gnrc packet from whom to fetch
 *
 * @return              netif header flags of @p pkt
 * @return              0, if no header is present
 */
uint8_t gnrc_netif_hdr_get_flag(gnrc_pktsnip_t* pkt);

/**
 * @brief   Extract the destination address out of a gnrc packet
 *
 * @param[in]   pkt                 gnrc packet from whom to extract
 * @param[out]  pointer_to_addr     pointer to address will be stored here
 *
 * @return                          length of destination address
 * @return                          -ENOENT, if no netif header is presented in @p pkt or if no
 *                                           destination address field presented in netif header.
 */
int gnrc_netif_hdr_get_dstaddr(gnrc_pktsnip_t* pkt, uint8_t** pointer_to_addr);

/**
 * @brief   Extract the source address out of a gnrc packet
 *
 * @param[in]   pkt                 gnrc packet from whom to extract
 * @param[out]  pointer_to_addr     pointer to address will be stored here
 *
 * @return                          length of source address
 * @return                          -ENOENT, if no netif header is presented in @p pkt or if no
 *                                           source address field presented in netif header.
 */
int gnrc_netif_hdr_get_srcaddr(gnrc_pktsnip_t* pkt, uint8_t** pointer_to_addr);

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_NETIF_HDR_H */
/** @} */
