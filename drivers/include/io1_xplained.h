/*
 * SPDX-FileCopyrightText: 2016 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    drivers_io1_xplained Atmel IO1 Xplained Extension board
 * @ingroup     drivers_sensors
 * @ingroup     drivers_saul
 * @brief       Device driver interface for the Atmel IO1 Xplained extension.
 *
 * This board contains several sensors and actuators:
 * * @ref drivers_at30tse75x
 * * TEMT6000 Light sensor (not yet supported by this driver)
 * * microSD card connector (not yet supported by this driver)
 * * one yellow LED
 * * 2 GPIO pins
 *
 * This board can be used with the following Atmel evaluation boards:
 * * @ref boards_samr21-xpro
 * * @ref boards_samd21-xpro
 * * @ref boards_saml21-xpro
 *
 * See the [datasheet]
 * (http://www.atmel.com/images/atmel-42078-io1-xplained-pro_user-guide.pdf)
 * for more information
 *
 * This driver provides @ref drivers_saul capabilities.
 * @{
 *
 * @file
 * @brief       Device driver interface for the Atmel IO1 Xplained extension.
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#include "saul.h"
#include "at30tse75x.h"
#include "sdcard_spi.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   IO1 Xplained driver return codes
 */
enum {
    IO1_XPLAINED_OK = 0,           /**< Initialization successful */
    IO1_XPLAINED_NOTEMP,           /**< Error during temperature sensor initialization */
    IO1_XPLAINED_NOSDCARD,         /**< Error during sdcard initialization */
    IO1_XPLAINED_NOLIGHT,          /**< Error during light sensor (ADC) initialization */
    IO1_XPLAINED_NOLED,            /**< Error during extension LED initialization */
    IO1_XPLAINED_NOGPIO1,          /**< Error during extension GPIO1 initialization */
    IO1_XPLAINED_NOGPIO2,          /**< Error during extension GPIO2 initialization */
    IO1_XPLAINED_READ_OK,          /**< Light sensor read successful */
    IO1_XPLAINED_READ_ERR          /**< Error when reading light sensor */
};

/**
 * @brief   Device initialization parameters
 */
typedef struct {
    uint8_t addr;                  /**< extension custom address */
} io1_xplained_params_t;

/**
 * @brief   Device descriptor for the IO1 Xplained extension.
 */
typedef struct {
    io1_xplained_params_t params;  /**< Initialization parameters */
    at30tse75x_t temp;             /**< On-board temperature sensor */
    sdcard_spi_t sdcard;           /**< On-board SD card */
} io1_xplained_t;

/**
 * @brief   Initialize the given IO1 Xplained extension
 *
 * @param[out] dev          Initialized device descriptor of IO1 Xplained extension
 * @param[in]  params       Device initialization parameters
 *
 * @return                  IO1_XPLAINED_OK on success
 * @return                  -IO1_XPLAINED_NOTEMP if temperature sensor initialization failed
 * @return                  -IO1_XPLAINED_NOSDCARD if sdcard initialization failed
 * @return                  -IO1_XPLAINED_NOLED if LED initialization failed
 * @return                  -IO1_XPLAINED_NOGPIO1 if GPIO1 initialization failed
 * @return                  -IO1_XPLAINED_NOGPIO2 if GPIO2 initialization failed
 */
int io1_xplained_init(io1_xplained_t *dev, const io1_xplained_params_t *params);

/**
 * @brief   Read light sensor level on the IO1 Xplained extension
 *
 * @param[out] light        Light level value (between 0 and 1023)
 *
 * @return                  IO1_XPLAINED_READ_OK on success
 * @return                  -IO1_XPLAINED_READ_ERR when the value cannot be read
 */
int io1_xplained_read_light_level(uint16_t *light);

#ifdef __cplusplus
}
#endif

/** @} */
