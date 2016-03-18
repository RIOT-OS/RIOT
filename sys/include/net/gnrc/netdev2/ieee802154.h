/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup net_gnrc
 * @brief
 * @{
 *
 * @file
 * @brief   netdev2 gnrc IEEE 802.15.4 glue code interface
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef GNRC_NETDEV2_IEEE802154_H_
#define GNRC_NETDEV2_IEEE802154_H_

#include "net/netdev2/ieee802154.h"
#include "net/gnrc/netdev2.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize gnrc handler for netdev2 IEEE 802.15.4 device
 *
 * @param[in] gnrc_netdev2  gnrc_netdev2 struct to initialize
 * @param[in] dev           netdev2 device to handle
 *
 * @return 1    on success
 * @return <=0  on error
 */
int gnrc_netdev2_ieee802154_init(gnrc_netdev2_t *gnrc_netdev2,
                                 netdev2_ieee802154_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* GNRC_IEEE802154_H_ */
/** @} */
