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

#ifndef NETDEV_DEFAULT_H_
#define NETDEV_DEFAULT_H_

#include "netdev/base.h"

#ifdef DOXYGEN
/**
 * @brief   Default device as a pointer of netdev_t.
 */
#define NETDEV_DEFAULT
#endif

#ifdef MODULE_AT86RF231
#include "at86rf231.h"

#ifndef NETDEV_DEFAULT
#define NETDEV_DEFAULT  ((netdev_t *)(&at86rf231_netdev))
#endif /* NETDEV_DEFAULT */
#endif /* MODULE_AT86RF231 */

#ifdef MODULE_CC110X
#include "cc110x.h"

#ifndef NETDEV_DEFAULT
#define NETDEV_DEFAULT ((netdev_t *)(&cc110x_dev))
#endif /* NETDEV_DEFAULT */
#endif /* MODULE_CC110X */

#ifdef MODULE_NATIVENET
#include "nativenet.h"

#ifndef NETDEV_DEFAULT
#define NETDEV_DEFAULT   (&nativenet_default_dev)
#endif /* NETDEV_DEFAULT */
#endif /* MODULE_NATIVENET */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* NETDEV_DEFAULT_H_ */
/** @} */
