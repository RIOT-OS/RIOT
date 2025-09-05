/*
 * SPDX-FileCopyrightText: 2020 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_ds3231
 * @{
 *
 * @file
 * @brief       Default configuration for DS3231 devices
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Default configuration parameters for the DS3231 driver
 * @{
 */
#ifndef DS3231_PARAM_I2C
#define DS3231_PARAM_I2C        I2C_DEV(0)
#endif
#ifndef DS3231_PARAM_OPT
#define DS3231_PARAM_OPT        (DS3231_OPT_BAT_ENABLE)
#endif
#ifndef DS3231_PARAM_INT_PIN
#define DS3231_PARAM_INT_PIN    (GPIO_UNDEF)    /**< Interrupt pin */
#endif

#ifndef DS3231_PARAMS
#if IS_USED(MODULE_DS3231_INT)
#define DS3231_PARAMS           { .bus = DS3231_PARAM_I2C, \
                                  .opt = DS3231_PARAM_OPT, \
                                  .int_pin = DS3231_PARAM_INT_PIN}
#else /* MODULE_DS3231_INT */
#define DS3231_PARAMS           { .bus = DS3231_PARAM_I2C, \
                                  .opt = DS3231_PARAM_OPT}
#endif /* MODULE_DS3231_INT */
#endif /* DS3231_PARAMS */
/** @} */

/**
 * @brief   DS3231 configuration
 */
static const ds3231_params_t ds3231_params[] =
{
    DS3231_PARAMS
};

#ifdef __cplusplus
}
#endif

/** @} */
