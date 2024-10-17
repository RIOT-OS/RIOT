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
#include "macros/utils.h"

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
#if IS_USED(MODULE_IEEE802154_SECURITY) || DOXYGEN
#define IEEE802154_SECURITY_EXTRA_STACKSIZE (128)
#else
#define IEEE802154_SECURITY_EXTRA_STACKSIZE (0)
#endif

/**
 * @brief   extra stack size if openDSME is enabled
 */
#if IS_USED(MODULE_OPENDSME)
#define IEEE802154_OPENDSME_EXTRA_STACKSIZE (THREAD_STACKSIZE_DEFAULT)
#else
#define IEEE802154_OPENDSME_EXTRA_STACKSIZE (0)
#endif

#ifndef IEEE802154_STACKSIZE_DEFAULT
/**
 * @brief   stack size of an ieee802154 device
 */
#define IEEE802154_STACKSIZE_DEFAULT    (MAX(520, GNRC_NETIF_STACKSIZE_DEFAULT) + \
                                         IEEE802154_SECURITY_EXTRA_STACKSIZE + \
                                         IEEE802154_OPENDSME_EXTRA_STACKSIZE)
#endif

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* INIT_DEVS_H */
