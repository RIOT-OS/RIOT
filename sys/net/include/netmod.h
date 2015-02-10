/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_netmod Global network module list
 * @ingroup     net
 * @brief       Global list of available network module
 * @{
 *
 * @file
 * @brief       List of all available network modules
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef __NETMOD_H_
#define __NETMOD_H_


typedef enum {
#if MODULE_UDP
    NETMOD_UDP,
#endif
#if MODULE_TCP
    NETMOD_TCP,
#endif

#if MODULE_IPV6
    NETMOD_IPV6,
#endif
#if MODULE_IPV4
    NETMOD_IPV4
#endif
#if MODULE_6LOWPAN
    NETMOD_6LOWPAN
#endif

#if MDULE_IEEE802154
    NETMOD_IEEE802154
#endif

    /* ... you get the idea ... */

    NETMOD_NUMOF        /**< shortcut for referencing the number of active protocols */
} netmod_t;

#endif /* __NETMOD_H_ */
/** @} */
