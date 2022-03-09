/*
 * Copyright (C) 2020 Viktor Gal
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_seesaw_soil Adafruit Seesaw Soil Moisture and Temperature Sensor
 * @ingroup     drivers_sensors
 * @ingroup     drivers_saul
 * @brief       Driver for the Adafruit Seesaw Soil Moisture and Temperature Sensor
 *
 *
 * This driver provides @ref drivers_saul capabilities.
 *
 * @{
 *
 * @file
 * @brief       Interface definition for the Adafruit Seesaw Soil sensor.
 *
 * @author      Viktor Gal <viktor.gal@maeth.com>
 */

#ifndef SEESAW_SOIL_H
#define SEESAW_SOIL_H

#include <stdint.h>

#include "periph/i2c.h"
#include "seesaw_soil_regs.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @defgroup drivers_seesaw_soil_config     Adafruit Seesaw Soil Moisture and Temperature Sensor driver compile configuration
 * @ingroup config_drivers_sensors
 * @{
 */
/**
 * @brief   Default I2C bus address of Adafruit Seesaw Soil devices
 *
 * The address value depends on the state of AD0 and AD1 Pins
 */
#ifndef CONFIG_SEESAW_SOIL_I2C_ADDRESS
#define CONFIG_SEESAW_SOIL_I2C_ADDRESS           (0x36)
#endif
/** @} */

/**
 * @brief   Seesaw Soil specific return values
 */
enum {
    SEESAW_SOIL_OK     = 0,     /**< everything went as expected */
    SEESAW_SOIL_NODEV  = -1,    /**< no SEESAW_SOIL device found on the bus */
    SEESAW_SOIL_NOBUS  = -2,    /**< errors while initializing the I2C bus */
    SEESAW_SOIL_BUSERR = -3     /**< error during I2C communication */
};

/**
 * @brief   Parameters needed for device initialization
 */
typedef struct {
    i2c_t i2c;               /**< bus the device is connected to */
    uint8_t addr;            /**< address on that bus */
} seesaw_soil_params_t;

/**
 * @brief   Device descriptor for Seesaw Soil sensors
 */
typedef struct {
    seesaw_soil_params_t params;     /**< Configuration parameters */
} seesaw_soil_t;

/**
 * @brief   Initialize the given Seesaw Soil device
 *
 * @param[out] dev          device descriptor of sensor to initialize
 * @param[in]  params       configuration parameters
 *
 * @return                  SEESAW_SOIL_OK on success
 * @return                  SEESAW_SOIL_NOBUS if initialization of I2C bus fails
 * @return                  SEESAW_SOIL_NODEV if no Seesaw Soil device found on bus
 */
int seesaw_soil_init(seesaw_soil_t *dev, const seesaw_soil_params_t *params);

/**
 * @brief   Convenience function for reading temperature
 *
 * @param[in]  dev          device descriptor of sensor
 * @param[out] temp         temperature [in 100 * degree centigrade]
 *
 * @return                  SEESAW_SOIL_OK on success
 * @return                  SEESAW_SOIL_BUSERR on I2C communication failures
 */
int seesaw_soil_temperature(const seesaw_soil_t *dev, int16_t *temp);

/**
 * @brief   Convenience function for reading ca
 *
 * @param[in]  dev          device descriptor of sensor
 * @param[out] moist        moisture using capacitive measurement.
 *                          Value is ranging from 200 (very dry) to 2000 (very wet).
 *
 * @return                  SEESAW_SOIL_OK on success
 * @return                  SEESAW_SOIL_BUSERR on I2C communication failures
 */
int seesaw_soil_moisture(const seesaw_soil_t *dev, uint16_t *moist);

#ifdef __cplusplus
}
#endif

#endif /* SEESAW_SOIL_H */
/** @} */
