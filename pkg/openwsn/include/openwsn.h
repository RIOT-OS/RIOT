/*
 * Copyright (C) 2018 Hamburg University of Applied Sciences
 *               2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_openwsn
 *
 * @{
 *
 * @file
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 */
#ifndef OPENWSN_H
#define OPENWSN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "thread.h"

/**
 * @brief   Default PANID for OpenWSN network
 */
#ifndef  OPENWSN_PANID
#define  OPENWSN_PANID  (0xCAFE)
#endif

/**
 * @brief   Default NID for OpenWSN network
 *
 * @note Only one netif is currently possible in OpenWSN.
 */
#ifndef  CONFIG_OPENWSN_NETIF_ID
#define  CONFIG_OPENWSN_NETIF_ID  (0x0001)
#endif

/**
 * @brief   Initializes OpenWSN thread
 *
 * @return  PID of OpenWSN thread
 * @return  -1 on initialization error
 */
int openwsn_bootstrap(void);

/**
 * @brief   get PID of OpenWsn thread.
 *
 * @return  PID of OpenWsn thread
 */
kernel_pid_t openwsn_get_pid(void);

#ifdef __cplusplus
}
#endif

#endif /* OPENWSN_H */
/** @} */
