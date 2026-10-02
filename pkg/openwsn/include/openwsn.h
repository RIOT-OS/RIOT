/*
 * SPDX-FileCopyrightText: 2018 HAW Hamburg
 * SPDX-FileCopyrightText: 2020 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

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

/** @} */
