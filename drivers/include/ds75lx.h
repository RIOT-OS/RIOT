/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    drivers_ds75lx Maxim DS75LX temperature sensor
 * @ingroup     drivers_sensors
 * @ingroup     drivers_saul
 * @brief       Device driver interface for the Maxim DS75LX temperature sensor
 *
 * This driver provides @ref drivers_saul capabilities.
 * @{
 *
 * @file
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#include "saul.h"
#include "periph/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Thermometer resolution
 */
typedef enum {
    DS75LX_RESOLUTION_9 = 0,        /**< 9 bits resolution, 25ms max conversion time */
    DS75LX_RESOLUTION_10,           /**< 10 bits resolution, 50ms max conversion time */
    DS75LX_RESOLUTION_11,           /**< 11 bits resolution, 100ms max conversion time */
    DS75LX_RESOLUTION_12,           /**< 12 bits resolution, 200ms max conversion time */
} ds75lx_resolution_t;

/**
 * @brief   Device initialization parameters
 */
typedef struct {
    i2c_t i2c;                      /**< I2C device which is used */
    uint8_t addr;                   /**< I2C address */
    ds75lx_resolution_t resolution; /**< Thermometer resolution */
} ds75lx_params_t;

/**
 * @brief   Device descriptor for the DS75LX sensor
 */
typedef struct {
    ds75lx_params_t params;         /**< Device initialization parameters */
} ds75lx_t;

/**
 * @brief   Status and error return codes
 */
enum {
    DS75LX_OK = 0,                  /**< everything was fine */
    DS75LX_ERR_I2C,                 /**< error when reading/writing I2C bus */
};

/**
 * @brief   Initialize the given DS75LX device
 *
 * @param[out] dev          Initialized device descriptor of DS75LX device
 * @param[in]  params       Initialization parameters
 *
 * @return                  DS75LX_OK on success
 * @return                  -DS75LX_ERR_I2C if an error occurred when reading/writing
 */
int ds75lx_init(ds75lx_t *dev, const ds75lx_params_t *params);

/**
 * @brief   Read temperature value from the given DS75LX device, returned in c°C
 *
 * @param[in] dev           Device descriptor of DS75LX device
 * @param[out] temperature  Temperature in c°C
 *
 * @return                  DS75LX_OK on success
 * @return                  -DS75LX_ERR_I2C if an error occurred when reading/writing
 */
int ds75lx_read_temperature(const ds75lx_t *dev, int16_t *temperature);

/**
 * @brief   Wakeup the sensor
 *
 * @param[in] dev           Device descriptor of DS75LX device
 *
 * @return                  DS75LX_OK on success
 * @return                  -DS75LX_ERR_I2C if an error occurred when reading/writing
 */
int ds75lx_wakeup(const ds75lx_t *dev);

/**
 * @brief   Shutdown the sensor
 *
 * @param[in] dev           Device descriptor of DS75LX device
 *
 * @return                  DS75LX_OK on success
 * @return                  -DS75LX_ERR_I2C if an error occurred when reading/writing
 */
int ds75lx_shutdown(const ds75lx_t *dev);

#ifdef __cplusplus
}
#endif

/** @} */
