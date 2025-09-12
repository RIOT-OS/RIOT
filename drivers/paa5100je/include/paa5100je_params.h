/*
 * SPDX-FileCopyrightText: 2025 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_paa5100je
 *
 * @{
 * @file
 * @brief       Default configuration
 *
 * @author      Leonard Herbst <leonard.herbst@tu-dresden.de>
 */

#include "board.h"
#include "paa5100je.h"
#include "paa5100je_constants.h"
#include "periph/spi.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Default quality threshold for motion data readout.
 */
#ifndef CONFIG_PAA5100JE_QUALITY_THRESHOLD
#  define CONFIG_PAA5100JE_QUALITY_THRESHOLD    (0x19)
#endif

/**
 * @brief Default timeout for motion data readout in milliseconds.
 */
#ifndef CONFIG_PAA5100JE_TIMEOUT_MS
#  define CONFIG_PAA5100JE_TIMEOUT_MS           (1000U)
#endif

/**
 * @brief Default denominator for scaling the motion data.
 */
#ifndef CONFIG_PAA5100JE_SCALE_DENOMINATOR
#  define CONFIG_PAA5100JE_SCALE_DENOMINATOR    (100)
#endif

/**
 * @name    Default configuration parameters for the PAA5100JE/PMW3901 driver.
 * @{
 */
#ifndef PAA5100JE_PARAM_SPI
#  define PAA5100JE_PARAM_SPI         (SPI_DEV(0))      /**< Default SPI device */
#endif

#ifndef PAA5100JE_PARAM_SPI_CLK
#  define PAA5100JE_PARAM_SPI_CLK     (SPI_CLK_400KHZ)  /**< Default SPI speed */
#endif

#ifndef PAA5100JE_PARAM_CS
#  define PAA5100JE_PARAM_CS          (GPIO_PIN(1, 2))  /**< Default SPI chip select pin */
#endif

#ifndef PAA5100JE_PARAM_VAR
#  define PAA5100JE_PARAM_VAR         PAA5100JE         /**< Default variant*/
#endif

/**
 * @brief Default sensor parameters.
 */
#ifndef PAA5100JE_PARAMS
#  define PAA5100JE_PARAMS            { .spi = PAA5100JE_PARAM_SPI,     \
                                        .clk = PAA5100JE_PARAM_SPI_CLK, \
                                        .cs  = PAA5100JE_PARAM_CS, \
                                        .var = PAA5100JE_PARAM_VAR }
#endif

/**
 * @brief Default driver SAUL registry information
 */
#ifndef PAA5100JE_SAUL_INFO
#  define PAA5100JE_SAUL_INFO         { .name = "Flow Sensor (PAA5100JE/PMW3901)" }
#endif

/**@}*/

/**
 * @brief Default configuration parameters
 */
static const paa5100je_params_t paa5100je_params[] =
{
    PAA5100JE_PARAMS
};

/**
 * @brief Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t paa5100je_saul_info[] =
{
    PAA5100JE_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

/** @} */
