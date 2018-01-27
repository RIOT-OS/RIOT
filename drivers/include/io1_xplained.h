/*
 * Copyright (C) 2016 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_io1_xplained Atmel IO1 Xplained Extension board
 * @ingroup     drivers_sensors
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
 * @{
 *
 * @file
 * @brief       Device driver interface for the Atmel IO1 Xplained extention.
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef IO1_XPLAINED_H
#define IO1_XPLAINED_H

#include "saul.h"
#include "at30tse75x.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   IO1 Xplained driver return codes
 */
enum {
    IO1_XPLAINED_OK = 0,           /**< Initialization successful */
    IO1_XPLAINED_NOTEMP,           /**< Error during temperature sensor initialization */
    IO1_XPLAINED_NOLED,            /**< Error during extension LED initialization */
    IO1_XPLAINED_NOGPIO1,          /**< Error during extension GPIO1 initialization */
    IO1_XPLAINED_NOGPIO2,          /**< Error during extension GPIO2 initialization */
    IO1_XPLAINED_READ_OK,          /**< Temperature read successful */
    IO1_XPLAINED_READ_ERR          /**< Error when reading temperature sensor */
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
} io1_xplained_t;

/**
 * @brief   Initialize the given IO1 Xplained extension
 *
 * @param[out] dev          Initialized device descriptor of IO1 Xplained extension
 * @param[in]  params       Device initialization parameters
 *
 * @return                  IO1_XPLAINED_OK on success
 * @return                  -IO1_XPLAINED_NOTEMP if temperature sensor initialization failed
 * @return                  -IO1_XPLAINED_NOLED if LED initialization failed
 * @return                  -IO1_XPLAINED_NOGPIO1 if GPIO1 initialization failed
 * @return                  -IO1_XPLAINED_NOGPIO2 if GPIO2 initialization failed
 */
int io1_xplained_init(io1_xplained_t *dev, const io1_xplained_params_t *params);

/**
 * @brief   Read temperature value from the given IO1 Xplained extension, returned in °C
 *
 * @param[in] dev           Device descriptor of IO1 Xplained to read from
 * @param[out] temperature  Temperature in °C
 *
 * @return                  IO1_XPLAINED_READ_OK on success
 * @return                  -IO1_XPLAINED_READ_ERR if temperature sensor read failed
 */
int io1_xplained_read_temperature(const io1_xplained_t *dev, float *temperature);

/**
 * @brief   Set the on-board led of the IO1 Xplained extension
 */
void io1_xplained_set_led(void);

/**
 * @brief   Clear the on-board led of the IO1 Xplained extension
 */
void io1_xplained_clear_led(void);

/**
 * @brief   Toggle the on-board led of the IO1 Xplained extension
 */
void io1_xplained_toggle_led(void);

#ifdef __cplusplus
}
#endif

#endif /* IO1_XPLAINED_H */
/** @} */
