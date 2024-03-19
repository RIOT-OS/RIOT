/*
 * Copyright (C) 2022 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    test_utils_netdev_eth_minimal Minimal netdev Ethernet device processing
 * @ingroup     sys
 *
 * @{
 * @file
 * @brief       Provides basic functionalities to interact with an Ethernet networking
 *              device which implements the @ref drivers_netdev_api.
 *
 * To use the functionalities, include the module `USEMODULE += test_utils_netdev_eth_minimal`.
 * The test application should provide:
 *  - device initialization, via the implementation of @ref netdev_eth_minimal_init_devs
 *  - number of devices to test, via the definition of @ref NETDEV_ETH_MINIMAL_NUMOF in `init_dev.h`
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 */

#ifndef TEST_UTILS_NETDEV_ETH_MINIMAL_H
#define TEST_UTILS_NETDEV_ETH_MINIMAL_H

#include "net/netdev.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef DOXYGEN
/**
 * @brief Maximum number of devices to handle.
 * @note Should be provided by the application via `init_dev.h`.
 */
#define NETDEV_ETH_MINIMAL_NUMOF
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
int netdev_eth_minimal_init_devs(netdev_event_cb_t cb);

/**
 * @brief   Initialize the module.
 *
 * @retval 0 on success
 * @retval != 0 on error
 */
int netdev_eth_minimal_init(void);

#ifdef __cplusplus
}
#endif

#endif /* TEST_UTILS_NETDEV_ETH_MINIMAL_H */
/** @} */
