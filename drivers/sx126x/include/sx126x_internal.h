/*
 * Copyright (C) 2021 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

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
#ifndef SX126X_INTERNAL_H
#define SX126X_INTERNAL_H

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

#ifdef __cplusplus
}
#endif

#endif /* SX126X_INTERNAL_H */
/** @} */
