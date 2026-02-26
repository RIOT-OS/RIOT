/*
 * SPDX-FileCopyrightText: 2026 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_amg88xx
 *
 * @{
 * @file
 * @brief       Default configuration
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 */

#include "board.h"
#include "saul_reg.h"

#include "amg88xx.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the AMG88xx sensor
 * @{
 */
#ifndef AMG88XX_PARAM_I2C_DEV
#  define AMG88XX_PARAM_I2C_DEV     I2C_DEV(0)              /**< I2C device */
#endif
#ifndef AMG88XX_PARAM_ADDR
#  define AMG88XX_PARAM_ADDR        AMG88XX_I2C_ADDRESS     /**< I2C address */
#endif
#if IS_USED(MODULE_PERIPH_GPIO_IRQ)
#ifndef AMG88XX_PARAM_INT_PIN
#  define AMG88XX_PARAM_INT_PIN     GPIO_UNDEF              /**< Interrupt pin */
#endif
#endif

#ifndef AMG88XX_PARAMS
#  if IS_USED(MODULE_PERIPH_GPIO_IRQ) || defined(DOXYGEN)
#    define AMG88XX_PARAMS          { .i2c_dev = AMG88XX_PARAM_I2C_DEV, \
                                      .address = AMG88XX_PARAM_ADDR, \
                                      .int_pin = AMG88XX_PARAM_INT_PIN }    /**< Default params */
#  else
#    define AMG88XX_PARAMS          { .i2c_dev = AMG88XX_PARAM_I2C_DEV, \
                                      .address = AMG88XX_PARAM_ADDR }
#  endif
#endif
#ifndef AMG88XX_SAUL_INFO
#  define AMG88XX_SAUL_INFO         { .name = "amg88xx" }                   /**< SAUL info */
#endif
/**@}*/

/**
 * @brief   Configure AMG88xx
 */
static const amg88xx_params_t amg88xx_params[] =
{
    AMG88XX_PARAMS
};

/**
 * @brief   Configure SAUL registry entries
 */
static const saul_reg_info_t amg88xx_saul_info[] =
{
    AMG88XX_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

/** @} */
