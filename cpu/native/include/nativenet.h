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
 * Copyright (C) 2013 Ludwig Ortmann
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup 	native_net
 * @ingroup		native_cpu
 * @{
 * @author  Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 */

#ifndef NATIVENET_H
#define NATIVENET_H

#include <net/ethernet.h>

#define RX_BUF_SIZE (10)
#define TRANSCEIVER_BUFFER_SIZE (3)

#ifndef NATIVE_MAX_DATA_LENGTH
#include "tap.h"
#define NATIVE_MAX_DATA_LENGTH (TAP_MAX_DATA)
#else
#warning be careful not to exceed (TAP_MAX_DATA) with NATIVE_MAX_DATA_LENGTH
#endif /* NATIVE_MAX_DATA_LENGTH */

/**
 * Initialize transceiver
 *
 * @param transceiver_pid the pid of the transceiver thread
 */
void nativenet_init(int transceiver_pid);

/**
 * Shutdown transceiver
 */
void nativenet_powerdown();

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
 * @return 1 on success, 0 otherwise
 */
uint8_t nativenet_send(radio_packet_t *packet);

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
radio_address_t nativenet_get_address();

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
int16_t nativenet_get_channel();

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
uint16_t nativenet_get_pan();

/**
 * Enable transceiver rx mode
 */
void nativenet_switch_to_rx();
/** @} */
#endif /* NATIVENET_H */
