/*
 * Copyright (C) 2019 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @ingroup drivers_nrf24l01p
 * @ingroup drivers_netdev
 * @ingroup net_gnrc_netif
 * @{
 *
 * @file
 * @brief   NRF24L01P adaptation for @ref net_gnrc_netif
 *
 * @author  Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author  Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author  Fabian Hüßler <fabian.huessler@ovgu.de>
 */
#ifndef GNRC_NETIF_NRF24L01P_H
#define GNRC_NETIF_NRF24L01P_H

#include "net/gnrc/netif.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Default protocol for data that is coming in
 */
#if IS_USED(MODULE_GNRC_SIXLOWPAN)
#define NRF24L01P_UPPER_LAYER_PROTOCOL         (GNRC_NETTYPE_SIXLOWPAN)
#else
#define NRF24L01P_UPPER_LAYER_PROTOCOL         (GNRC_NETTYPE_UNDEF)
#endif

/**
 * @brief Creates an NRF24L01P network interface
 *
 * @param[in] stack     The stack for the network interface's thread
 * @param[in] stacksize Size of @p stack
 * @param[in] priority  Priority for the network interface's thread
 * @param[in] name      Name for the network interface. May be NULL
 * @param[in] dev       Device for the interface
 *
 * @see @ref gnrc_netif_create()
 *
 * @return  The network interface on success.
 */
gnrc_netif_t *gnrc_netif_nrf24l01p_create(char *stack, int stacksize,
                                          char priority, char *name,
                                          netdev_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* GNRC_NETIF_NRF24L01P_H */
/** @} */
