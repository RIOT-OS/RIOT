/*
 * Copyright (C) 2020 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_auto_init_gnrc_netif
 * @{
 *
 * @file
 * @brief       common netif device initialization definitions
 *
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 */

#ifndef INIT_DEVS_H
#define INIT_DEVS_H

#include "thread.h"
#include "msg.h"
#include "net/gnrc/netif/conf.h"    /* <- GNRC_NETIF_MSG_QUEUE_SIZE */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   stack size of a netif thread
 *
 *          Message queue was previously allocated on the stack, reduce
 *          stack size by default msg queue size to keep the RAM use the same
 */
#ifndef GNRC_NETIF_STACKSIZE_DEFAULT
#define GNRC_NETIF_STACKSIZE_DEFAULT    (THREAD_STACKSIZE_DEFAULT - 128)
#endif

/**
 * @brief   extra stack size if ieee802154 security is enabled
 *
 * You may increase this value if you experience a stack overflow
 * with IEEE 802.15.4 security enabled.
 */
#ifdef MODULE_IEEE802154_SECURITY
#define IEEE802154_SECURITY_EXTRA_STACKSIZE (128)
#else
#define IEEE802154_SECURITY_EXTRA_STACKSIZE (0)
#endif

#ifndef IEEE802154_STACKSIZE_DEFAULT
/**
 * @brief   stack size of an ieee802154 device
 */
#define IEEE802154_STACKSIZE_DEFAULT    (GNRC_NETIF_STACKSIZE_DEFAULT + \
                                         IEEE802154_SECURITY_EXTRA_STACKSIZE)
#endif

#ifdef __cplusplus
}
#endif

#endif /* INIT_DEVS_H */
/** @} */
