/*
 * Copyright (C) 2022 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    test_utils_netdev_ieee802154_minimal Minimal netdev IEEE 802.15.4 device processing
 * @ingroup     sys
 *
 * @{
 * @file
 * @brief       Provides basic functionalities to interact with an
 *              IEEE 802.15.4 networking device which implements the
 *              @ref drivers_netdev_api.
 *
 * To use the functionalities, include the module
 * `USEMODULE += test_utils_netdev_ieee802154_minimal`.
 * The test application should provide:
 *  - device initialization, via the implementation of @ref netdev_ieee802154_minimal_init_devs
 *  - number of devices to test, via the definition of @ref NETDEV_IEEE802154_MINIMAL_NUMOF
 *    in `init_dev.h`
 *
 * @author      Kevin Weiss <kevin.weiss@haw-hamburg.de>
 */


#include "net/netdev.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef DOXYGEN
/**
 * @brief Maximum number of devices to handle.
 * @note Should be provided by the application via `init_dev.h`.
 */
#define NETDEV_IEEE802154_MINIMAL_NUMOF
#endif

/**
 * @brief   Device-under-test initialization function.
 * @note    Should be implemented by the test application
 *
 * @param[in] cb    Callback to be set to @ref netdev::event_callback
 *
 * @retval 0 on success
 * @retval != 0 on error
 */
int netdev_ieee802154_minimal_init_devs(netdev_event_cb_t cb);

/**
 * @brief   Initialize the module.
 *
 * @retval 0 on success
 * @retval != 0 on error
 */
int netdev_ieee802154_minimal_init(void);

/**
 * @brief   Send a IEEE 802.15.4 frame
 *          This is wrapper for the internal netdev send function, that ensures
 *          all netdev functions are called from the same thread. It is safe
 *          to call this function from anywhere.
 *
 * @param[in] dev    Pointer to the netdev descriptor.
 * @param[in] pkt    Packet to be sent.
 *
 * @retval 0 on success
 * @retval != 0 on error
 */
int netdev_ieee802154_minimal_send(struct netdev *dev, iolist_t *pkt);

/**
 * @brief   Get an option from netdev minimal.
 *          This is wrapper for the internal netdev get function, that ensures
 *          all netdev functions are called from the same thread. It is safe
 *          to call this function from anywhere.
 *
 * @param[in]  dev      Pointer to the netdev descriptor.
 * @param[in]  opt      The netopt option
 * @param[out] data     Buffer to store the option
 * @param[in]  max_len  Maximum length of the buffer
 *
 * @retval 0 on success
 * @retval != 0 on error
 */
int netdev_ieee802154_minimal_get(struct netdev *dev, netopt_t opt, void *data, size_t max_len);

/**
 * @brief   Set an option to netdev minimal.
 *          This is wrapper for the internal netdev set function, that ensures
 *          all netdev functions are called from the same thread. It is safe
 *          to call this function from anywhere.
 *
 * @param[in]  dev      Pointer to the netdev descriptor.
 * @param[in]  opt      The netopt option
 * @param[in]  data     Pointer to the data to be set
 * @param[in]  len      Length of the data
 *
 * @retval 0 on success
 * @retval != 0 on error
 */
int netdev_ieee802154_minimal_set(struct netdev *dev, netopt_t opt, void *data, size_t len);

#ifdef __cplusplus
}
#endif

/** @} */
