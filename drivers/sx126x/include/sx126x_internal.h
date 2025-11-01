/*
 * SPDX-FileCopyrightText: 2021 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    drivers_sx126x_internal SX1261/2/8 and LLCC68 internal functions
 * @ingroup     drivers_sx126x
 * @brief       Internal functions for the SX1261/2/8 and LLCC68
 *
 * @{
 *
 * @file
 *
 * @author      José I. Alamos <jose.alamos@haw-hamburg.de>
 */

#include <assert.h>
#include "sx126x.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Check whether the device model is sx1261
 *
 * @param[in] dev                      Device descriptor of the driver
 *
 * @retval    true if the device is sx1261
 * @retval    false otherwise
 */
static inline bool sx126x_is_sx1261(sx126x_t *dev)
{
    assert(dev);
    if (SX126X_SINGLE) {
        return IS_USED(MODULE_SX1261);
    }
    else {
        return dev->params->type == SX126X_TYPE_SX1261;
    }
}

/**
 * @brief   Check whether the device model is sx1262
 *
 * @param[in] dev                      Device descriptor of the driver
 *
 * @retval    true if the device is sx1262
 * @retval    false otherwise
 */
static inline bool sx126x_is_sx1262(sx126x_t *dev)
{
    assert(dev);
    if (SX126X_SINGLE) {
        return IS_USED(MODULE_SX1262);
    }
    else {
        return dev->params->type == SX126X_TYPE_SX1262;
    }
}

/**
 * @brief   Check whether the device model is llcc68
 *
 * @param[in] dev                      Device descriptor of the driver
 *
 * @retval    true if the device is llcc68
 * @retval    false otherwise
 */
static inline bool sx126x_is_llcc68(sx126x_t *dev)
{
    assert(dev);
    if (SX126X_SINGLE) {
        return IS_USED(MODULE_LLCC68);
    }
    else {
        return dev->params->type == SX126X_TYPE_LLCC68;
    }
}

/**
 * @brief   Check whether the device model is sx1268
 *
 * @param[in] dev                      Device descriptor of the driver
 *
 * @retval    true if the device is sx1268
 * @retval    false otherwise
 */
static inline bool sx126x_is_sx1268(sx126x_t *dev)
{
    assert(dev);
    if (SX126X_SINGLE) {
        return IS_USED(MODULE_SX1268);
    }
    else {
        return dev->params->type == SX126X_TYPE_SX1268;
    }
}

/**
 * @brief   Check whether the device model is stm32wl (onboard radio)
 *
 * @param[in] dev                      Device descriptor of the driver
 *
 * @retval    true if the device is stm32wl
 * @retval    false otherwise
 */
static inline bool sx126x_is_stm32wl(sx126x_t *dev)
{
    assert(dev);
    if (SX126X_SINGLE) {
        return IS_USED(MODULE_SX126X_STM32WL);
    }
    else {
        return dev->params->type == SX126X_TYPE_STM32WL;
    }
}

/**
 * @brief   Get the current chip state
 *
 * @param[in]   dev                     Device descriptor of the driver
 *
 * @return  Chip state
 */
sx126x_chip_modes_t sx126x_get_state(const sx126x_t *dev);

/**
 * @brief   Set the internal chip state
 *
 * @param[in]   dev                     Device descriptor of the driver
 * @param[in]   state                   State to set
 */
void sx126x_set_state(sx126x_t *dev, sx126x_chip_modes_t state);

#ifdef __cplusplus
}
#endif

/** @} */
