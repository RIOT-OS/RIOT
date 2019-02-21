/*
 * Copyright (C) 2017 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_grove_ledbar Grove ledbar
 * @ingroup     drivers_actuators
 * @ingroup     drivers_saul
 * @brief       Driver for the Grove ledbar
 *
 * This driver provides @ref drivers_saul capabilities.
 *
 * @{
 * @file
 * @brief       Interface for the Grove ledbar driver
 *
 * @author      Sebastian Meiling <s@mlng.net>
 */

#ifndef GROVE_LEDBAR_H
#define GROVE_LEDBAR_H

#include <stdint.h>

#include "my9221.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name	Ledbar direction, either red to green, or vice versa.
 * @{
 */
#define GROVE_LEDBAR_R2G        MY9221_DIR_FWD
#define GROVE_LEDBAR_G2R        MY9221_DIR_REV
/** @} */

/**
 * @brief 	Maximum value for ledbar level
 */
#define GROVE_LEDBAR_MAX        UINT8_MAX

/**
 * @brief   Parameters needed for device initialization
 */
typedef my9221_params_t grove_ledbar_params_t;

/**
 * @brief   Device descriptor for grove ledbar, alias for MY9221 LED controller
 */
typedef my9221_t grove_ledbar_t;

/**
 * @brief 	Initialize the given driver
 *
 * @pre `dev != NULL` and `params != NULL`
 *
 * @param[out]  dev         device descriptor grove ledbar
 * @param[in]   params      configuration parameters
 *
 * @return                  0 on success, otherwise error
 */
int grove_ledbar_init(grove_ledbar_t *dev, const grove_ledbar_params_t *params);

/**
 * @brief 	Set overall (brightness) level distributed over all LEDs
 *
 * @pre `dev != NULL`
 *
 * @param[in]   dev         device descriptor grove ledbar
 * @param[in]   level       overall brightness level
 */
void grove_ledbar_set(grove_ledbar_t *dev, uint8_t level);

/**
 * @brief 	Clear ledbar, i.e. set all LEDs off
 *
 * @pre `dev != NULL`
 *
 * @param[in]   dev         device descriptor grove ledbar
 */
void grove_ledbar_clear(grove_ledbar_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* GROVE_LEDBAR_H */
/** @} */
