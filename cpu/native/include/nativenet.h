/**
 * nativenet transceiver interface
 *
 * A configurable transceiver for the native port.
 *
 * At the moment only the tap interface is supported, but more network
 * layers are intended. So the "configurable" part is a lie for now ;-)
 * The effect of calls like nativenet_set_channel depend on the
 * network layer.
 *
 * Copyright (C) 2013, 2014 Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    native_net
 * @ingroup     native_cpu
 * @{
 * @author  Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 */

#ifndef NATIVENET_H
#define NATIVENET_H

#include <net/ethernet.h>

#include "kernel_types.h"
#include "netdev/base.h"

#ifndef RX_BUF_SIZE
#define RX_BUF_SIZE (10)
#endif
#ifndef TRANSCEIVER_BUFFER_SIZE
#define TRANSCEIVER_BUFFER_SIZE (10)
#endif

/**
 * @brief   Number of registrable netdev_rcv_data_cb_t callbacks per nativenet
 *          device
 */
#define NATIVENET_DEV_CB_MAX  (128)

#ifndef NATIVE_MAX_DATA_LENGTH
#include "tap.h"
#ifdef MODULE_SIXLOWPAN
#define NATIVE_MAX_DATA_LENGTH (127)
#else
#define NATIVE_MAX_DATA_LENGTH (TAP_MAX_DATA)
#endif
#else
#warning be careful not to exceed (TAP_MAX_DATA) with NATIVE_MAX_DATA_LENGTH
#endif /* NATIVE_MAX_DATA_LENGTH */

/**
 * @brief   Implementation of netdev_driver_t for nativenet
 */
extern const netdev_driver_t nativenet_driver;

/**
 * @brief   Default @netdev API device
 */
extern netdev_t nativenet_default_dev;

/**
 * Initialize @ref sys_transceiver and @ref nativenet_default_dev
 *
 * @param transceiver_pid the pid of the transceiver thread
 */
void nativenet_init(kernel_pid_t transceiver_pid);

/**
 * Shutdown transceiver
 */
void nativenet_powerdown(void);

/**
 * Enable/disable monitor mode
 *
 * @param mode 0 off, 1 on
 */
void nativenet_set_monitor(uint8_t mode);

/**
 * Send a packet
 *
 * @param packet a radio packet
 * @return -1 if the operation failed, the number of transmitted bytes
 *         up to INT8_MAX otherwise
 */
int8_t nativenet_send(radio_packet_t *packet);

/**
 * Set transceiver address
 *
 * @param address the address
 * @return the address
 */
radio_address_t nativenet_set_address(radio_address_t address);

/**
 * Get transceiver address
 *
 * @return the address
 */
radio_address_t nativenet_get_address(void);

/**
 * @brief Sets the IEEE long address of the nativenet transceiver.
 *
 * @param[in] addr The desired address.
 *
 * @return The set address after calling.
 */
uint64_t nativenet_set_address_long(uint64_t addr);

/**
 * @brief Gets the current IEEE long address of the nativenet
 * transceiver.
 *
 * @return The current IEEE long address.
 */
uint64_t nativenet_get_address_long(void);

/**
 * Set transceiver channel
 *
 * @param channel the channel
 * @return the channel
 */
int16_t nativenet_set_channel(uint8_t channel);

/**
 * Get transceiver channel
 *
 * @return the channel
 */
int16_t nativenet_get_channel(void);

/**
 * Set transceiver pan
 *
 * @param channel the pan
 * @return the pan
 */
uint16_t nativenet_set_pan(uint16_t pan);

/**
 * Get transceiver pan
 *
 * @return the pan
 */
uint16_t nativenet_get_pan(void);

/**
 * Enable transceiver rx mode
 */
void nativenet_switch_to_rx(void);
/** @} */
#endif /* NATIVENET_H */
