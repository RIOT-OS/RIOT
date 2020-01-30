/*
 * Copyright (C) 2020 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net
 *
 * @{
 *
 * @file
 * @brief   Unique IDs to identify netif drivers.
 *
 * @author  Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */
#ifndef NET_NETIF_IDS_H
#define NET_NETIF_IDS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Unique IDs for netif drivers.
 */
typedef enum {
    AT86RF2XX_DEVUID,
} netif_id_t;

#ifdef __cplusplus
}
#endif

#endif /* NET_NETIF_IDS_H */
/** @} */
