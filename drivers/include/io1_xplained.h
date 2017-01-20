/*
 * Copyright (C) 2016 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_io1_xplained IO1_XPLAINED
 * @ingroup     drivers_sensors
 * @brief       Device driver interface for the IO1 Xplained extension.
 * @{
 *
 * @file
 * @brief       Device driver interface for the IO1 Xplained extention.
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
 * @brief Device descriptor for the IO1 Xplained extension.
 */
typedef struct {
    at30tse75x_t temp  /**< On-board temperature sensor */;
} io1_xplained_t;


/**
 * @brief Device initialization parameters
 */
typedef struct {
    uint8_t addr;      /**< extension custom address */
} io1_xplained_params_t;

/**
 * @brief export SAUL endpoints
 * @{
 */
extern const saul_driver_t io1_xplained_temperature_saul_driver;
/** @} */

/**
 * @brief auto-initialize all configured IO1 Xplained extensions
 */
void io1_xplained_auto_init(void);

/**
 * @brief Initialize the given IO1 Xplained extension
 *
 * @param[out] dev          Initialized device descriptor of IO1 Xplained extension
 * @param[in]  addr         Custom address of the extension board
 *
 * @return                  0 on success
 * @return                  -1 if given I2C is not enabled in board config
 */
int io1_xplained_init(io1_xplained_t *dev, uint8_t addr);

/**
 * @brief Read temperature value from the given IO1 Xplained extension, returned in °C
 *
 * @param[in] dev           Device descriptor of IO1 Xplained to read from
 * @param[out] temperature  Temperature in °C
 *
 * @return                  0 on success
 * @return                  -1 if device's I2C is not enabled in board config
 */
int io1_xplained_read_temperature(io1_xplained_t *dev, float *temperature);

/**
 * @brief Set the on-board led of the IO1 Xplained extension
 *
 * @return                0 on success
 * @return                -1 if extension GPIO is not enabled in board config
 */
int io1_xplained_set_led(void);

/**
 * @brief Clear the on-board led of the IO1 Xplained extension
 *
 * @return                0 on success
 * @return                -1 if extension GPIO is not enabled in board config
 */
int io1_xplained_clear_led(void);

/**
 * @brief Toggle the on-board led of the IO1 Xplained extension
 *
 * @return                0 on success
 * @return                -1 if extension GPIO is not enabled in board config
 */
int io1_xplained_toggle_led(void);

#ifdef __cplusplus
}
#endif

#endif /* IO1_XPLAINED_H */
/** @} */
