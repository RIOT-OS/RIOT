/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup        drivers_nrf51822_nrfmin NRF Minimal Radio Driver
 * @ingroup         drivers_netdev
 * @brief           Minimal driver for the NRF51822 radio
 *
 * This driver enables the use of the NRF51822 radio in a IEEE802.15.4 like
 * fashion. In the current state, the driver is only be meant to be used with
 * the netdev/netapi based network stack, while only being able to communicate
 * with other NRF51822 devices using the same driver.
 *
 * The driver is using a Nordic proprietary physical layer, configured to for a
 * bitrate of 2Mbit. The payload length is set to a maximum length of 250 byte.
 * The proprietary frame format used has the following format:
 *
 *    byte0 | byte1 - byte2 | byte3 - byte4 | byte5   byte6 | byte7 - byteN
 *   ------ | ------------- | ------------- | ------------- | -------------
 *   length |   src_addr    |   dst_addr    |    proto      |   payload...
 *
 * An IEEE802.15.4 like behavior is reflected in the following way: the driver
 * configures the radio device to use a fixed 5 byte addressing scheme. On this
 * addresses, the first byte is set to a constant value, the same for all
 * devices that use this driver. The next two bytes are set to the configured
 * PAN ID, hereby simulating the use of PAN IDs. The last two bytes are set to
 * a 16-bit short address, simulating IEEE802.15.4 short addresses.
 *
 * There is no support for EUIDs. Further there is no support for anything else
 * than IEEE802.15.4 data frames, so no PAN coordinators, etc.
 *
 * The driver supports:
 *   - short address (16-bit)
 *   - using CPU-ID for default address
 *   - address broadcast (broadcast address is ff:ff)
 *   - PAN IDs (0 to 0xffff), PAN ID broadcast is not supported
 *   - setting of channel (0 to 0x3f)
 *   - setting of TX power (+4dBm to -20dBm)
 *   - packet type labeling
 *   - setting device state (RX, SLEEP)
 *
 * @{
 *
 * @file
 * @brief           Interface definition for the nrfmin NRF51822 radio driver
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef NRFMIN_H_
#define NRFMIN_H_

#include "net/gnrc/netdev.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Default PAN ID used after initialization
 */
#define NRFMIN_DEFAULT_PAN          (0x0550)

/**
 * @brief   Default channel set after initialization
 */
#define NRFMIN_DEFAULT_CHANNEL      (1U)                /* 2401MHz */

/**
 * @brief   Default transmission power used
 */
#define NRFMIN_DEFAULT_TXPOWER      (0)                 /* 0dBm */

/**
 * @brief   Reference to the netdev driver interface
 */
extern const gnrc_netdev_driver_t nrfmin_driver;

/**
 * @brief   Initialize the NRF51822 radio
 *
 * The initialization uses static configuration values.
 *
 * @param[out] dev      pointer to the netdev device descriptor
 *
 * @return              0 on success
 * @return              -ENODEV if @p dev is invalid
 */
int nrfmin_init(gnrc_netdev_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* NRFMIN_H_ */
/** @} */
