/*
 * SPDX-FileCopyrightText: 2015 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    drivers_netdev_eth Ethernet drivers
 * @ingroup     drivers_netdev_api
 * @{
 *
 * @file
 * @brief       Definitions for netdev common ethernet code
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#include <stdint.h>

#include "net/netdev.h"
#include "net/netopt.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Flags for use in @ref netdev_eth_rx_info_t::flags
 * @{
 */
#define NETDEV_ETH_RX_INFO_FLAG_TIMESTAMP       (0x01)  /**< Timestamp valid */
/** @} */

/**
 * @brief   Received frame status information for Ethernet devices
 */
typedef struct {
    /**
     * @brief   Time of the reception of the start of frame delimiter in
     *          nanoseconds since epoch
     */
    uint64_t timestamp;
    uint8_t flags;      /**< Flags e.g. used to mark other fields as valid */
} netdev_eth_rx_info_t;

/**
 * @brief   Fallback function for netdev ethernet devices' _get function
 *
 * Supposed to be used by netdev drivers as default case.
 *
 * @warning Driver *MUST* implement NETOPT_ADDRESS case!
 *
 * @param[in]   dev     network device descriptor
 * @param[in]   opt     option type
 * @param[out]  value   pointer to store the option's value in
 * @param[in]   max_len maximal amount of byte that fit into @p value
 *
 * @return              number of bytes written to @p value
 * @return              <0 on error
 */
int netdev_eth_get(netdev_t *dev, netopt_t opt, void *value, size_t max_len);

/**
 * @brief   Fallback function for netdev ethernet devices' _set function
 *
 * @param[in] dev       network device descriptor
 * @param[in] opt       option type
 * @param[in] value     value to set
 * @param[in] value_len the length of @p value
 *
 * @return              number of bytes used from @p value
 * @return              <0 on error
 */
int netdev_eth_set(netdev_t *dev, netopt_t opt, const void *value, size_t value_len);

#ifdef __cplusplus
}
#endif

/** @} */
