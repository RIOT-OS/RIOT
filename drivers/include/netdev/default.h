/*
 * Copyright (C) 2014 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  netdev
 * @{
 *
 * @file        netdev/default.h
 * @brief       Supplies the users with default values for the use of
 *              @ref netdev
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#ifndef __NETDEV_DEFAULT_H_
#define __NETDEV_DEFAULT_H_

#include "netdev/base.h"

/**
 * @brief   Default device as a pointer of netdev_t.
 */
#ifdef MODULE_AT86RF231
#include "at86rf231.h"

#ifndef NETDEV_DEFAULT
#define NETDEV_DEFAULT  ((netdev_t *)(&at86rf231_netdev))
#endif /* NETDEV_DEFAULT */
#endif /* MODULE_AT86RF231 */

#ifdef MODULE_NATIVENET

#include "nativenet.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NETDEV_DEFAULT
#define NETDEV_DEFAULT   (&nativenet_default_dev)
#endif /* NETDEV_DEFAULT */

#ifdef __cplusplus
}
#endif

#endif /* MODULE_NATIVENET */

#endif /* __NETDEV_DEFAULT_H_ */
/** @} */
