/*
 * Copyright (C) 2020 Locha Inc
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_cc26x2_cc13x2
 * @ingroup         drivers_netdev
 * @brief           Driver for using the CC13x2's radio in IEEE802.15.4g mode.
 *
 *
 * @{
 *
 * @file
 * @brief           Driver interface for using the CC13x2 in IEEE 802.15.4g mode.
 *
 * @author          Jean Pierre Dudey <jeandudey@hotmail.com>
 */

#ifndef CC26X2_CC13X2_RF_H
#define CC26X2_CC13X2_RF_H

#include "net/ieee802154/radio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   IEEE 802.15.4 radio timer configuration
 *
 * This radio relies on a dedicated hardware timer to maintain IFS,
 * the default timer may be overwritten in the board configuration.
 */
#ifndef CC26X2_CC13X2_RF_TIMER
#define CC26X2_CC13X2_RF_TIMER TIMER_DEV(2)
#endif

typedef struct {
    uint8_t state;                /**< current state of the radio */
} cc26x2_cc13x2_rf_t;

/**
 * @brief   Initialize radio hardware (RF Core)
 */
void cc26x2_cc13x2_rf_init(void);

/**
 * @brief   Setup the radio interface.
 *
 * @pre @p dev != NULL
 *
 * @param[in] dev Device descriptor.
 */
void cc26x2_cc13x2_rf_setup(cc26x2_cc13x2_rf_t *dev);

/**
 * @brief   Setup cc26x2_cc13x2 in order to be used with the IEEE 802.15.4 Radio HAL
 *
 * @note    This functions MUST be called before @ref cc26x2_cc13x2_rf_init.
 *
 * @param[in] hal  pointer to the HAL descriptor associated to the device.
 */
void cc26x2_cc13x2_rf_hal_setup(ieee802154_dev_t *hal);

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* CC26X2_CC13X2_RF_H */

/*@}*/
