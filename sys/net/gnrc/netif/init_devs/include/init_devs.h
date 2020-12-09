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

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   extra stack size if ieee802154 security is enabled
 *
 * You may increase this value if you experience a stack overflow
 * with IEEE 802.15.4 security enabled.
 */
#define IEEE802154_SECURITY_EXTRA_STACKSIZE (128)

#ifndef IEEE802154_STACKSIZE_DEFAULT
#ifdef MODULE_IEEE802154_SECURITY
#define IEEE802154_STACKSIZE_DEFAULT    (THREAD_STACKSIZE_DEFAULT + \
                                        IEEE802154_SECURITY_EXTRA_STACKSIZE)
#else
/**
 * @brief   stack size of an ieee802154 device
 */
#define IEEE802154_STACKSIZE_DEFAULT    (THREAD_STACKSIZE_DEFAULT)
#endif
#endif

#ifdef __cplusplus
}
#endif

#endif /* INIT_DEVS_H */
/** @} */
