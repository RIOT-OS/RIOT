/*
 * SPDX-FileCopyrightText: 2015 Ludwig Knüpfer,
 * SPDX-FileCopyrightText: 2015 Christian Mehlis
 * SPDX-FileCopyrightText: 2016-2017 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    drivers_dht DHT Family of Humidity and Temperature Sensors
 * @ingroup     drivers_sensors
 * @ingroup     drivers_saul
 * @brief       Device driver for the DHT Family of humidity
 *              and temperature sensors
 *
 * This driver provides @ref drivers_saul capabilities.
 *
 * @{
 *
 * @file
 * @brief       Device driver interface for the DHT family of humidity
 *              and temperature sensors
 *
 * @author      Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de
 * @author      Christian Mehlis <mehlis@inf.fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#include <errno.h>
#include <stdint.h>

#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Data type for storing DHT sensor readings
 */
typedef struct {
    uint16_t humidity;      /**< relative humidity in deci-percent */
    uint16_t temperature;   /**< temperature in deci-Celsius */
} dht_data_t;

/**
 * @brief   Device type of the DHT device
 */
typedef enum {
    DHT11,                  /**< Older DHT11 variants with either 1 °C or
                              *  0.1 °C resolution */
    DHT11_2022,             /**< New DHT11 variant with 0.01 °C resolution */
    DHT22,                  /**< DHT22 device identifier */
    DHT21 = DHT22,          /**< DHT21 device identifier */
    AM2301 = DHT22,         /**< AM2301 device identifier */
} dht_type_t;

/**
 * @brief   Configuration parameters for DHT devices
 */
typedef struct {
    gpio_t pin;             /**< GPIO pin of the device's data pin */
    dht_type_t type;        /**< type of the DHT device */
    gpio_mode_t in_mode;    /**< input pin configuration, with or without pull
                             *   resistor */
} dht_params_t;

/**
 * @brief   Device descriptor for DHT sensor devices
 */
typedef struct {
    dht_params_t params;    /**< Device parameters */
    dht_data_t last_val;    /**< Values of the last measurement */
} dht_t;

/**
 * @brief   Initialize a new DHT device
 *
 * @param[out] dev      device descriptor of a DHT device
 * @param[in]  params   configuration parameters
 *
 * @retval  0           Success
 * @retval -EXDEV       A low level on the input after the sensor's startup
 *                      time indicates that either no sensor or pull-up
 *                      resistor is connected, or the sensor is physically
 *                      poorly connected or powered.
 * @retval -ENODEV      The sensor did not respond to the transmission of a
 *                      start signal. Likely there were a pull-up resistor but
 *                      no sensor connected on the data line.
 */
int dht_init(dht_t *dev, const dht_params_t *params);

/**
 * @brief   get a new temperature and/or humidity value from the device
 *
 * @note    if reading fails or checksum is invalid, no new values will be
 *          written into the result values
 *
 * @param[in]  dev      device descriptor of a DHT device
 * @param[out] temp     temperature value [in °C * 10^-1],
 *                      may be NULL if not needed
 * @param[out] hum      relative humidity value [in percent * 10^-1],
 *                      may be NULL if not needed
 *
 * @retval 0            Success
 * @retval -ENODEV      The sensor did not respond to the transmission of a
 *                      start signal. Likely the RESPAWN_TIMEOUT is
 *                      insufficient.
 * @retval -EIO         The received and the expected checksum didn't match.
 * @retval -ENOSYS      Unable to parse the received data. Likely the data
 *                      format is not implemented.
 * @retval -ERANGE      Temperature low byte >= 10. Likely misconfigured
 *                      device type (DHT11_2022).
 */
int dht_read(dht_t *dev, int16_t *temp, int16_t *hum);

#ifdef __cplusplus
}
#endif

/** @} */
