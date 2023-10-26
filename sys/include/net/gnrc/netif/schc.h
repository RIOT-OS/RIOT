/*
 * Copyright (C) 2022 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup net_gnrc_netif
 * @{
 *
 * @file
 * @brief   SCHC definitions for @ref net_gnrc_netif
 *
 * @author  Martine S. Lenders <m.lenders@fu-berlin.de>
 */
#ifndef NET_GNRC_NETIF_SCHC_H
#define NET_GNRC_NETIF_SCHC_H

#include "kernel_defines.h"

#if IS_USED(MODULE_LIBSCHC_GNRC)
#include "libschc/gnrc_types.h"
#endif /* IS_USED(MODULE_LIBSCHC_GNRC) */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   SCHC interface descriptor
 */
#if IS_USED(MODULE_LIBSCHC_GNRC)
typedef libschc_gnrc_netif_t gnrc_netif_schc_t;
#endif /* IS_USED(MODULE_LIBSCHC_GNRC) */

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_NETIF_SCHC_H */
/** @} */
