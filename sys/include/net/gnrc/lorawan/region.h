/*
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup net_gnrc_lorawan
 * @{
 *
 * @file
 * @brief   GNRC LoRaWAN region specific functions
 *
 * @author  José Ignacio Alamos <jose.alamos@haw-hamburg.de>
 */
#ifndef NET_GNRC_LORAWAN_REGION_H
#define NET_GNRC_LORAWAN_REGION_H

#include "modules.h"
#include "net/gnrc/lorawan.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Default LoRaWAN channels
 */
static const uint32_t gnrc_lorawan_default_channels[] = {
#if (IS_ACTIVE(CONFIG_LORAMAC_REGION_EU_868))
    868100000UL,
    868300000UL,
    868500000UL
#elif (IS_ACTIVE(CONFIG_LORAMAC_REGION_IN_865))
    865062500UL,
    865402500UL,
    865985000UL
#endif
};

#define GNRC_LORAWAN_DEFAULT_CHANNELS_NUMOF \
    ARRAY_SIZE(gnrc_lorawan_default_channels) /**< Number of default channels */

/**
 * @brief Process Channel Frequency list frame
 *
 * @param[in] mac pointer to the MAC descriptor
 * @param[in] cflist the CFList to be processed
 */
void gnrc_lorawan_process_cflist(gnrc_lorawan_t *mac, uint8_t *cflist);

/**
 * @brief Get the datarate of the first reception windows
 *
 * @param[in] dr_up the datarate of the transmission
 * @param[in] dr_offset the offset of the first reception window
 *
 * @return datarate
 */
uint8_t gnrc_lorawan_rx1_get_dr_offset(uint8_t dr_up, uint8_t dr_offset);

/**
 * @brief Check if a datarate is valid in the current region
 *
 * @param[in] dr the datarate to be checked
 *
 * @return true if datarate is valid
 * @return false otherwise
 */
bool gnrc_lorawan_validate_dr(uint8_t dr);

/**
 * @brief Check if a TX power is valid in the current region
 *
 * @param[in] tx_power the TX power to be checked
 *
 * @retval true if TX power is valid
 * @retval false otherwise
 */
bool gnrc_lorawan_validate_tx_power(uint8_t tx_power);

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_LORAWAN_REGION_H */
/** @} */
