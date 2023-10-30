/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup net_gnrc_netif
 * @{
 *
 * @file
 * @brief   IEEE 802.15.4 adaption for @ref net_gnrc_netif
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef NET_GNRC_NETIF_IEEE802154_H
#define NET_GNRC_NETIF_IEEE802154_H

#include "net/gnrc/netif.h"
#include "net/gnrc/netif/hdr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Creates an IEEE 802.15.4 network interface
 *
 * @param[out] netif    The interface. May not be `NULL`.
 * @param[in] stack     The stack for the network interface's thread.
 * @param[in] stacksize Size of @p stack.
 * @param[in] priority  Priority for the network interface's thread.
 * @param[in] name      Name for the network interface. May be NULL.
 * @param[in] dev       Device for the interface
 *
 * @see @ref gnrc_netif_create()
 *
 * @return  0 on success
 * @return  negative number on error
 */
int gnrc_netif_ieee802154_create(gnrc_netif_t *netif, char *stack, int stacksize,
                                 char priority, const char *name, netdev_t *dev);

/**
 * @brief   Convert GNRC netif header to an IEEE 802.15.4 header
 *
 * @param[out]  mhr     Destination IEEE 802.15.4 header
 * @param[in]   netif   IEEE 802.15.4 network interface
 * @param[in]   hdr     GNRC netif header
 *
 * @return  IEEE 802.15.4 header on success
 * @return  negative number on error
 */
int gnrc_netif_hdr_to_ieee802154(uint8_t *mhr, const gnrc_netif_t *netif,
                                 const gnrc_netif_hdr_t *hdr);

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_NETIF_IEEE802154_H */
/** @} */
