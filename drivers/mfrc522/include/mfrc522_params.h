/*
 * Copyright (C) 2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup drivers_actuators
 * @brief   Default configuration for the MFRC522 controller
 *
 * @{
 *
 * @author  Hendrik van Essen <hendrik.ve@fu-berlin.de>
 * @file
 */
#ifndef MFRC522_PARAMS_H
#define MFRC522_PARAMS_H

#include <stdbool.h>

#include "periph/spi.h"

#include "mfrc522_regs.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @name Default configuration parameters for MFRC522 driver
 * @{
 */
#ifndef MFRC522_PARAM_SPI_DEV
#define MFRC522_PARAM_SPI_DEV   SPI_DEV(0)       /**< Default SPI device */
#endif
#ifndef MFRC522_PARAM_SPI_CLK
#define MFRC522_PARAM_SPI_CLK   SPI_CLK_5MHZ     /**< Default SPI speed */
#endif
#ifndef MFRC522_PARAM_SCK_PIN
#define MFRC522_PARAM_SCK_PIN   GPIO_PIN(0, 18)  /**< Default SCK pin */
#endif
#ifndef MFRC522_PARAM_MISO_PIN
#define MFRC522_PARAM_MISO_PIN  GPIO_PIN(0, 19)  /**< Default MISO pin */
#endif
#ifndef MFRC522_PARAM_MOSI_PIN
#define MFRC522_PARAM_MOSI_PIN  GPIO_PIN(0, 23)  /**< Default MOSI pin */
#endif
#ifndef MFRC522_PARAM_CS_PIN
#define MFRC522_PARAM_CS_PIN    GPIO_PIN(0, 5)   /**< Default CS pin */
#endif
#ifndef MFRC522_PARAM_RST_PIN
#define MFRC522_PARAM_RST_PIN   GPIO_PIN(0, 17)  /**< Default RST pin */
#endif

#ifndef MFRC522_PARAMS
#define MFRC522_PARAMS                      \
    {                                       \
        .spi_dev  = MFRC522_PARAM_SPI_DEV,  \
        .spi_clk  = MFRC522_PARAM_SPI_CLK,  \
        .sck_pin  = MFRC522_PARAM_SCK_PIN,  \
        .miso_pin = MFRC522_PARAM_MISO_PIN, \
        .mosi_pin = MFRC522_PARAM_MOSI_PIN, \
        .cs_pin   = MFRC522_PARAM_CS_PIN,   \
        .rst_pin  = MFRC522_PARAM_RST_PIN,  \
    }  /**< Struct with default configuration parameters */
#endif /* MFRC522_PARAMS */
/**@}*/

/**
 * @brief Allocate some memory to store the actual configuration
 */
static const mfrc522_params_t mfrc522_params[] =
{
    MFRC522_PARAMS
};

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* MFRC522_PARAMS_H */
