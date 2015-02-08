/*
 * Copyright (C) 2014 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  unittests
 * @{
 *
 * @file        netdev_dummy.h
 * @brief       Provides a virtual device that understands the netdev_base API
 *              for testing.
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#ifndef __UNITTESTS_NETDEV_DUMMY_H_
#define __UNITTESTS_NETDEV_DUMMY_H_

#include <stdlib.h>

#include "netdev/base.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef UNITTESTS_NETDEV_DUMMY_MAX_DEV
/**
 * @brief   Maximum number of devices you can initialize with this module
 */
#define UNITTESTS_NETDEV_DUMMY_MAX_DEV          (2)
#endif /* UNITTESTS_NETDEV_DUMMY_MAX_DEV */

#ifndef UNITTESTS_NETDEV_DUMMY_MAX_PACKET
/**
 * @brief   Maximum packet length for the virtual test devices.
 */
#define UNITTESTS_NETDEV_DUMMY_MAX_PACKET       (12)
#endif /* UNITTESTS_NETDEV_DUMMY_MAX_PACKET */

#ifndef UNITTESTS_NETDEV_DUMMY_MAX_ADDR_LEN
/**
 * @brief   The maximum length of the addresses for the test devices
 */
#define UNITTESTS_NETDEV_DUMMY_MAX_ADDR_LEN     (2)
#endif /* UNITTESTS_NETDEV_DUMMY_MAX_ADDR_LEN */

#ifndef UNITTESTS_NETDEV_DUMMY_MAX_LONG_ADDR_LEN
/**
 * @brief   The maximum length of the addresses for the test devices
 */
#define UNITTESTS_NETDEV_DUMMY_MAX_LONG_ADDR_LEN    (4)
#endif /* UNITTESTS_NETDEV_DUMMY_MAX_LONG_ADDR_LEN */

#if UNITTESTS_NETDEV_DUMMY_MAX_LONG_ADDR_LEN < UNITTESTS_NETDEV_DUMMY_MAX_ADDR_LEN
#error "UNITTESTS_NETDEV_DUMMY_MAX_ADDR_LEN < UNITTESTS_NETDEV_DUMMY_MAX_ADDR_LEN!\n"
#endif

#if UNITTESTS_NETDEV_DUMMY_MAX_LONG_ADDR_LEN > 64
#warning "UNITTESTS_NETDEV_DUMMY_MAX_LONG_ADDR_LEN > 64 is not recommended"
#endif

#ifndef UNITTESTS_NETDEV_DUMMY_MAX_CB
/**
 * @brief   Maximum number of callbacks per device
 */
#define UNITTESTS_NETDEV_DUMMY_MAX_CB       (4)
#endif /* UNITTESTS_NETDEV_DUMMY_MAX_CB */

/**
 * @brief   Implementation of the driver for the virtual test device
 */
extern const netdev_driver_t unittest_netdev_dummy_driver;

/**
 * @brief   Available devices
 */
extern netdev_t unittest_netdev_dummy_devs[UNITTESTS_NETDEV_DUMMY_MAX_DEV];

/**
 * @brief   Fires a receive event on a virtual test device
 *
 * @param[in] dev           Device you want to fire the receive event on
 * @param[in] src           Source address of the received packet
 * @param[in] src_len       Length of *src*
 * @param[in] dest          Destination address of the received packet
 * @param[in] dest_len      Length of *dest*
 * @param[in] data          Content of the received packet
 * @param[in] data_len      Length of *data*
 *
 * @return  0 on success
 * @return  -EAFNOSUPPORT, if src_len or dest_len are neither
 *          UNITTESTS_NETDEV_DUMMY_MAX_LONG_ADDR_LEN nor
 *          UNITTESTS_NETDEV_DUMMY_MAX_ADDR_LEN
 * @return  -ECANCELED, receive event aborted since one of the registered
 *          callbacks had an error.
 * @return  -EINVAL, if *src* or *dest* are NULL or
 *          *data* is NULL and *data_len* is not NULL
 * @return  -EMSGSIZE, if *data_len* > UNITTESTS_NETDEV_DUMMY_MAX_PACKET
 * @return  -ENODEV, if device was not initialized by
 *          unittest_netdev_dummy_driver::init()
 */
int unittest_netdev_dummy_fire_rcv_event(netdev_t *dev, void *src,
        size_t src_len, void *dest, size_t dest_len, void *data,
        size_t data_len);

/**
 * @brief   Checks if the data transmitted by
 *          unittest_netdev_dummy_driver::send_data() is as expected.
 *
 * @param[in] dev               Device you want to check the transmitted data for
 * @param[in] expected_dest     The expected destination address.
 * @param[in] expected_dest_len The expected length of the destination address.
 * @param[in] expected_data     The expected data
 * @param[in] expected_data_len The expected length of the data
 *
 * @note    Please remember that the device prepends the *upper_layer_hdrs*
 *          passed to unittest_netdev_dummy_driver::send_data()
 *
 * @return  0 on success and everything is as expected
 * @return  1 on success and something is not as expected
 * @return  -ENODEV, if *dev* was not initialized by
 *          unittest_netdev_dummy_driver::init()
 * @return  -EINVAL, if *expected_data* is NULL and *expected_data_len is not 0 or
 *          if
 */
int unittest_netdev_dummy_check_transmitted(netdev_t *dev,
        void *expected_dest,
        size_t expected_dest_len,
        void *expected_data,
        size_t expected_data_len);

/**
 * @brief   Get last event notified by dev::driver::event
 *
 * @param[in] dev   Device you want to get the last event from.
 *
 * @return  The last event as given as parameter to dev::driver::event
 * @return  ENODEV, if *dev* was not found
 */
uint32_t unittest_netdev_dummy_get_last_event(netdev_t *dev);

/**
 * @brief   Resets all dummy devices to their initial state
 */
void unittest_netdev_dummy_init(void);

#ifdef __cplusplus
}
#endif

#endif /* __UNITTESTS_NETDEV_DUMMY_H_ */

/**
 * @}
 */
