/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @defgroup    cpu_native_ng_tapnet   TAP interface as netdev device.
 * @ingroup     native_cpu
 *
 * @brief   Allows for usage of multiple TAP interfaces as ethernet netdev
 *          devices.
 * @{
 *
 * @file
 * @brief       Definitions for @ref cpu_native_ng_tapnet
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */


#ifndef NG_TAPNET_H_
#define NG_TAPNET_H_

#include <inttypes.h>

#include "kernel_types.h"
#include "net/ng_netdev.h"
#include "net/ng_ethernet/hdr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Definition of the TAP device
 * @extends ng_netdev_t
 * @internal
 */
typedef struct {
    ng_netdev_driver_t *driver;         /**< pointer to the devices interface */
    ng_netdev_event_cb_t event_cb;      /**< netdev event callback */
    kernel_pid_t mac_pid;               /**< the driver's thread's PID */
    int tap_fd;                         /**< file descriptor for the TAP */
    uint8_t addr[NG_ETHERNET_ADDR_LEN]; /**< The MAC address of the TAP */
    uint8_t promiscous;                 /**< Flag for promiscous mode */
} ng_tapnet_t;

/**
 * @brief   Reference to the TAP driver interface
 */
extern const ng_netdev_driver_t ng_tapnet_driver;

/**
 * @brief   Reference to the TAP device
 */
extern ng_tapnet_t ng_tapnet; /* XXX: this is only here since I do not know how
                               * to get the device in the interrupt handler */

/**
 * @brief   Initialize a given TAP device
 *
 * @param[out] dev      TAP device to initialize
 * @param[in] name      Name of the TAP interface.
 *                      If @p name is an empty string, the kernel chooses a name.
 *                      If @p name is an existing device, that device is used.
 *                      Otherwise a device named "name" is created
 * @param[in] name_len  Length of @p name.
 *
 * @return  0 on success
 * @return  -ENODEV on invalid device descriptor
 * @return  -ENXIO on error with TAP device creation
 */
int ng_tapnet_init(ng_tapnet_t *dev, char *name, size_t name_len);

#ifdef __cplusplus
}
#endif

#endif /* NG_TAPNET_H_ */
/**
 * @}
 */
