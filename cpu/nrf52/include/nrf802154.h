/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_nrf52_802154 IEEE802.15.4 Driver for nRF52840 SoCs
 * @ingroup     drivers_netdev
 * @brief       Driver for using the nRF52's radio in IEEE802.15.4 mode
 *
 * ## Implementation state ##
 * Netdev events supported:
 *
 * - NETDEV_EVENT_RX_COMPLETE
 * - NETDEV_EVENT_TX_COMPLETE
 *
 * Transmission options not yet implemented:
 * - Send acknowledgement for packages
 * - Request acknowledgement
 * - Retransmit unacked packages
 * - Carrier Sense Multiple Access (CSMA) and Implementation of Clear Channel
 *   Assessment Control (CCACTRL)
 *
 * @{
 *
 * @file
 * @brief       Driver interface for using the nRF52 in IEEE802.15.4 mode
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Semjon Kerner <semjon.kerner@fu-berlin.de>
 */

#ifndef NRF802154_H
#define NRF802154_H

#include "net/ieee802154/radio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Device descriptor for NRF802154 transceiver
 */
typedef struct nrf802154 nrf802154_t;

/**
 * @defgroup drivers_nrf52_802154_conf  nrf802154 driver compile configuration
 * @ingroup drivers_nrf52_802154
 * @ingroup config_drivers_netdev
 * @{
 */

/**
 * @brief NRF802154 default CCA threshold value for CCACTRL register.
 *
 * @note  This value was copied from the Nordic reference driver configuration
 */
#ifndef CONFIG_NRF802154_CCA_THRESH_DEFAULT
#define CONFIG_NRF802154_CCA_THRESH_DEFAULT 0x14
#endif
/** @} */

/**
 * @brief   IEEE 802.15.4 radio timer configuration
 *
 *          this radio relies on a dedicated hardware timer to maintain IFS
 *          the default timer may be overwritten in the board configuration
 */
#ifndef NRF802154_TIMER
#define NRF802154_TIMER TIMER_DEV(1)
#endif

/**
 * @brief   Setup NRF802154 in order to be used with the IEEE 802.15.4 Radio HAL
 *
 * @note    This functions MUST be called before @ref nrf802154_init.
 *
 * @param[in] hal  pointer to the HAL descriptor associated to the device.
 */
void nrf802154_hal_setup(ieee802154_dev_t *hal);

/**
 * @brief Initialize the NRF52840 radio.
 *
 * @return 0 on success
 * @return negative errno on error
 */
int nrf802154_init(void);

/**
 * @brief   Setup a NRF802154 radio device
 *
 * @param[out] dev          Device descriptor
 */
void nrf802154_setup(nrf802154_t *dev);

#endif /* NRF802154_H */
/** @} */
