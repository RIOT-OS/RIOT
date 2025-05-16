/*
 * Copyright (C) 2022 Inria
 * Copyright (C) 2020-2022 Université Grenoble Alpes
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_sx1280
 * @{
 * @file
 * @brief       Default configuration
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @author      Aymeric Brochier <aymeric.brochier@univ-grenoble-alpes.fr>
 *
 */


#include "board.h"
#include "sx1280.h"
#include "sx1280_constants.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters
 *
 * Default values are adapted for mbed shield used with to nucleo64 boards
 * @see https://github.com/donsez/RIOT/blob/pr/sx1280/drivers/sx1280/include/sx1280_hal.h
 * @see 'board/common/nucleo64/include/arduino_pinmap.h'
 *
 * ARDUINO_PIN_10 -> GPIO_PIN(PORT_B, 6) -> GPIO_PIN(1, 6)
 * ARDUINO_PIN_7 -> GPIO_PIN(PORT_A, 8) -> GPIO_PIN(0, 8)
 * ARDUINO_PIN_6 -> GPIO_PIN(PORT_B, 10) -> GPIO_PIN(1, 10)
 * ARDUINO_PIN_9 -> GPIO_PIN(PORT_C, 7) -> GPIO_PIN(2, 7)
 *
 * @{
 */

#ifndef USE_RX_CONTINUOUS_MODE
#define USE_RX_CONTINUOUS_MODE                             /**< default RX MODE */
#endif

#ifdef USE_RX_CONTINUOUS_MODE
#define SX1280_RX_MODE                      SX1280_RX_CONTINUOUS_MODE   /**< continuous RX MODE */
#else
#define SX1280_RX_MODE                      SX1280_RX_SINGLE_MODE       /**< single RX MODE */
#endif

#ifndef SX1280_PARAM_SPI
#define SX1280_PARAM_SPI                    SPI_DEV(0)      /**< default SPI device */
#endif

#ifndef SX1280_PARAM_SPI_CLK
#define SX1280_PARAM_SPI_CLK                SPI_CLK_5MHZ    /**< default SPI speed */
#endif

#ifndef SX1280_PARAM_SPI_MODE
#define SX1280_PARAM_SPI_MODE               SPI_MODE_0      /**< default SPI mode for sx1280 */
#endif

#ifndef SX1280_PARAM_SPI_NSS
#define SX1280_PARAM_SPI_NSS                GPIO_PIN(1, 6)  /**< SPI NSS pin */
#endif

#ifndef SX1280_PARAM_RESET
#define SX1280_PARAM_RESET                  GPIO_PIN(0, 8)  /**< Reset pin */
#endif

#ifndef SX1280_PARAM_DIO0
#define SX1280_PARAM_DIO0                   GPIO_PIN(1, 10) /**< DIO0 */
#endif

#ifndef SX1280_PARAM_DIO1
#define SX1280_PARAM_DIO1                   GPIO_PIN(2, 7) /**< DIO1 */
#endif


/**
 * @brief Default sx1280 parameters
 */
#define SX1280_PARAMS             { .spi = SX1280_PARAM_SPI,            \
                                    .spi_mode = SX1280_PARAM_SPI_MODE,  \
                                    .spi_clk = SX1280_PARAM_SPI_CLK,    \
                                    .nss_pin = SX1280_PARAM_SPI_NSS,    \
                                    .reset_pin = SX1280_PARAM_RESET,    \
                                    .dio0_pin = SX1280_PARAM_DIO0,      \
                                    .dio1_pin = SX1280_PARAM_DIO1,      \
}

/**@}*/

/**
 * @brief   Configuration struct
 */
static const sx1280_params_t sx1280_params[] =
{
    SX1280_PARAMS
};

#ifdef __cplusplus
}
#endif

/** @} */
