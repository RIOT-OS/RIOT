/*
 * Copyright (C) 2017 TU Berlin DAI Labor
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup   driver_cc1200
 * @{
 *
 * @file
 * @brief     cc1200 gnrc glue code interface
 *
 * @author      Anon Mall <anon.mall@gt-arc.com>
 */

//#include "net/gnrc/netdev.h"
#include "cc1200-netdev.h"

#ifndef GNRC_CC1200_H
#define GNRC_CC1200_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief initialize cc1200 gnrc glue code
 *
 * @param[in] gnrc_netdev  gnrc_netdev state structure to initialize
 * @param[in] dev           cc1200 device structure to setup
 *
 * @return 1    on sucess
 * @return <=0  on error
 */
//int gnrc_netdev_cc1200_init(gnrc_netdev_t *gnrc_netdev, netdev_cc1200_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* GNRC_CC1200_H */
/** @} */
