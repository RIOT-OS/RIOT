/*
 * Copyright (C) 2019 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @ingroup drivers_nrf24l01p_ng
 * @ingroup drivers_netdev
 * @ingroup net_gnrc_netif
 * @{
 *
 * @file
 * @brief   NRF24L01+ (NG) adaptation for @ref net_gnrc_netif
 *
 * @author  Fabian Hüßler <fabian.huessler@ovgu.de>
 */
#ifndef GNRC_NETIF_NRF24L01P_NG_H
#define GNRC_NETIF_NRF24L01P_NG_H

#include "net/gnrc/netif.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NRF24L01P_NG_UPPER_LAYER_PROTOCOL
#if IS_USED(MODULE_GNRC_SIXLOWPAN)
#define NRF24L01P_NG_UPPER_LAYER_PROTOCOL   (GNRC_NETTYPE_SIXLOWPAN)
#else
/**
 * @brief type of layer 2 payload
 */
#define NRF24L01P_NG_UPPER_LAYER_PROTOCOL   (GNRC_NETTYPE_UNDEF)
#endif
#endif

/**
 * @brief Creates an NRF24L01+ (NG) network interface
 *
 * @param[out] netif    The interface. May not be `NULL`.
 * @param[in] stack     The stack for the network interface's thread
 * @param[in] stacksize Size of @p stack
 * @param[in] priority  Priority for the network interface's thread
 * @param[in] name      Name for the network interface. May be NULL
 * @param[in] dev       Device for the interface
 *
 * @see @ref gnrc_netif_create()
 *
 * @return  0 on success
 * @return  negative number on error
 */
int gnrc_netif_nrf24l01p_ng_create(gnrc_netif_t *netif, char *stack,
                                   int stacksize, char priority, char *name,
                                   netdev_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* GNRC_NETIF_NRF24L01P_NG_H */
/** @} */
