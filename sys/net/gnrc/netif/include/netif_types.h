/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup net_netif
 * @{
 *
 * @file
 * @brief   GNRC-specfic type definitions for @ref net_netif
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef NETIF_TYPES_H
#define NETIF_TYPES_H

#include "kernel_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NETIF_INVALID   (KERNEL_PID_UNDEF)  /**< Invalid interface */

typedef kernel_pid_t netif_t;   /**< GNRC-representation of a network interface */

#ifdef __cplusplus
}
#endif

#endif /* NETIF_TYPES_H */
/** @} */
