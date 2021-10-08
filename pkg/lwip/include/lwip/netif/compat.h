/*
 * Copyright (C) 2021 Google LLC
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_lwip
 *
 * @{
 *
 * @file
 * @brief   lwIP definition based on common netif_t struct for network
 *          stack-independent API.
 *
 * @author  Erik Ekman <eekman@google.com>
 */
#ifndef LWIP_NETIF_COMPAT_H
#define LWIP_NETIF_COMPAT_H

#include "lwip/netif.h"
#include "net/netif.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Representation of a network interface
 */
typedef struct {
    netif_t common_netif;                /**< network interface descriptor */
    struct netif lwip_netif;             /**< lwIP interface data */
} lwip_netif_t;

#ifdef __cplusplus
}
#endif

#endif /* LWIP_NETIF_COMPAT_H */
/** @} */
