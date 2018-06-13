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
 * @brief   lwIP-specfic type definitions for @ref net_netif
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef NETIF_TYPES_H
#define NETIF_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lwip/netif.h"

#define NETIF_INVALID   (NULL)  /**< Invalid interface */

typedef struct netif *netif_t;  /**< lwIP-representation of a network interface */

#ifdef __cplusplus
}
#endif

#endif /* NETIF_TYPES_H */
