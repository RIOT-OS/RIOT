/*
 * Copyright (C) 2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_mcp9808 MCP9808 temperature sensor
 * @ingroup     drivers_sensors
 * @ingroup     drivers_saul
 * @brief       Device driver interface for the MCP9808 temperature sensor
 *
 * This driver support the following features:
 * - Set temperature resolution during init. Resolution can only be defined
 *   using the default parameters of a sensor.
 * - Use polling to read ambient temperature
 * - Shutdown and wake up of the sensor
 *
 * See the [datasheet](http://ww1.microchip.com/downloads/en/DeviceDoc/25095A.pdf)
 * for details.
 *
 * This driver provides @ref drivers_saul capabilities.
 * @{
 *
 * @file
 * @brief       Device driver interface for the MCP9808 sensor.
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef MCP9808_H
#define MCP9808_H

#include "saul.h"
#include "periph/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Resolution of measured ambient temperature
 */
typedef enum {
    MCP9808_RES_0_5 = 0,            /**< 0.5°C */
    MCP9808_RES_0_25,               /**< 0.25°C */
    MCP9808_RES_0_125,              /**< 0.125°C */
    MCP9808_RES_0_0625              /**< 0.0625°C (default) */
} mcp9808_resolution_t;

/**
 * @brief   Device initialization parameters
 */
typedef struct {
    i2c_t i2c_dev;                      /**< I2C device which is used */
    uint8_t i2c_addr;                   /**< I2C address */
    mcp9808_resolution_t resolution;    /**< temperature resolution */
} mcp9808_params_t;

/**
 * @brief   Device descriptor for the MCP9808 sensor
 */
typedef struct {
    mcp9808_params_t params;        /**< Device initialization parameters */
} mcp9808_t;

/**
 * @brief   Status and error return codes
 */
enum {
    MCP9808_OK = 0,                 /**< everything was fine */
    MCP9808_ERR_NODEV,              /**< did not detect MCP9808 */
    MCP9808_ERR_I2C,                /**< error when reading/writing over I2C */
};

/**
 * @brief   Initialize the given MCP9808 device
 *
 * @param[out] dev          Initialized device descriptor of MCP9808 device
 * @param[in]  params       Initialization parameters
 *
 * @return                  MCP9808_OK on success
 * @return                  -MCP9808_ERR_NODEV if not a MCP9808 at given address
 * @return                  -MCP9808_ERR_I2C if an error occured on I2C bus.
 */
int mcp9808_init(mcp9808_t *dev, const mcp9808_params_t *params);

/**
 * @brief   Read temperature value from the given MCP9808 device, returned in d°C
 *
 * @param[in] dev           Device descriptor of MCP9808 device
 * @param[out] temperature  Temperature in d°C

 * @return                  MCP9808_OK on success
 * @return                  -MCP9808_ERR_I2C if an error occured on I2C bus.
 */
int mcp9808_read_temperature(const mcp9808_t *dev, int16_t *temperature);

/**
 * @brief   Wake up the device.
 *
 * After calling this function a delay of 250ms must be used before reading
 * the temperature.
 *
 * @param[in] dev           Device descriptor of MCP9808 device

 * @return                  MCP9808_OK on success
 * @return                  -MCP9808_ERR_I2C if an error occured on I2C bus.
 */
int mcp9808_wakeup(const mcp9808_t *dev);

/**
 * @brief   Shutdown the device
 *
 * Calling this function will put the device in very low power mode.
 *
 * @param[in] dev           Device descriptor of MCP9808 device

 * @return                  MCP9808_OK on success
 * @return                  -MCP9808_ERR_I2C if an error occured on I2C bus.
 */
int mcp9808_shutdown(const mcp9808_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* MCP9808_H */
/** @} */
