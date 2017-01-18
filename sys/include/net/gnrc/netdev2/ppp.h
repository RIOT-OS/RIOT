/*
 * Copyright (C) Fundación Inria Chile 2017
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  net_gnrc
 * @{
 *
 * @file
 * @brief       GNRC to PPP netdev2 glue code interface
 *
 * @author      José Ignacio Alamos <jose.alamos@inria.cl>
 */

#ifndef GNRC_NETDEV2_PPP_H_
#define GNRC_NETDEV2_PPP_H_

#include "net/netdev2/ppp.h"
#include "net/gnrc/netdev2.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Initialize GNRC handler for netdev2 PPP devices
 *
 * @param[out] gnrc_netdev2     gnrc_netdev2 struct to initialize
 * @param[in]  dev              PPP device to handle
 */
void gnrc_netdev2_ppp_init(gnrc_netdev2_t *gnrc_netdev2, netdev2_ppp_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* GNRC_NETDEV2_PPP_H_ */
/** @} */
