/*
 * SPDX-FileCopyrightText: 2026 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_ads131m02
 * @{
 *
 * @file
 * @brief       Default configuration for the ADS131M02 ADC
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#include "board.h"
#include "container.h"
#include "ads131m02.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the ADS131M02 driver
 * @{
 */
#ifndef ADS131M02_PARAM_SPI
/**
 * @brief   Default SPI port
 */
#  define ADS131M02_PARAM_SPI           (SPI_DEV(0))
#endif
#ifndef ADS131M02_PARAM_SPI_CLK
/**
 * @brief   Default SPI clock speed
 */
#  define ADS131M02_PARAM_SPI_CLK       (SPI_CLK_1MHZ)
#endif
#ifndef ADS131M02_PARAM_CS
/**
 * @brief   SPI CS pin must be defined
 */
#  define ADS131M02_PARAM_CS            (GPIO_UNDEF)
#endif
#ifndef ADS131M02_PARAM_READY
/**
 * @brief   DRDY pin must be defined
 */
#  define ADS131M02_PARAM_READY        (GPIO_UNDEF)
#endif
#ifndef ADS131M02_PARAM_SYNC
/**
 * @brief   SYNC pin must be defined
 */
#  define ADS131M02_PARAM_SYNC         (GPIO_UNDEF)
#endif
#ifndef ADS131M02_PARAM_READY_PULLUP
/**
 * @brief   Must be set to true if GRDY pin is pulled up with an external 100-kOhm resistor
 */
#  define ADS131M02_PARAM_READY_PULLUP 0
#endif

#ifndef ADS131M02_PARAMS
/**
 * @brief   Default ADS131M02 parameters
 */
#  define ADS131M02_PARAMS             { .spi       = ADS131M02_PARAM_SPI,     \
                                         .spi_clk   = ADS131M02_PARAM_SPI_CLK, \
                                         .cs_pin    = ADS131M02_PARAM_CS,      \
                                         .ready_pin = ADS131M02_PARAM_READY,   \
                                         .sync_pin  = ADS131M02_PARAM_SYNC, \
                                         .ready_pin_pullup = ADS131M02_PARAM_READY_PULLUP }
#endif
/** @} */

/**
 * @brief   Number of ADS131M02 instances
 */
#define ADS131M02_NUMOF                 (ARRAY_SIZE(ads131m02_params))

/**
 * @brief   ADS131M02 configuration
 */
static const ads131m02_params_t ads131m02_params[] =
{
    ADS131M02_PARAMS
};

#ifdef __cplusplus
}
#endif

/** @} */
