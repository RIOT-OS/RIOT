/*
 * Copyright (C) 2016 Bas Stottelaar <basstottelaar@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_si70xx Si7006/13/20/21 temperature and humidity sensors
 * @ingroup     drivers_sensors
 * @brief       Driver for the Si7006/13/20/21 temperature and humidity sensor.
 * @{
 *
 * @file
 * @brief       Interface definition of the Si70xx driver.
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 */

#ifndef SI70XX_H
#define SI70XX_H

#include "periph/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Driver return codes
 */
enum {
    SI70XX_OK,                 /**< All OK */
    SI70XX_ERR_NODEV,          /**< No valid device found on I2C bus */
    SI70XX_ERR_I2C,            /**< An error occured when reading/writing on I2C bus */
};

/**
 * @brief Device initialization parameters.
 */
typedef struct {
    i2c_t i2c_dev;              /**< I2C bus the sensor is connected to */
    uint8_t address;            /**< sensor address */
} si70xx_params_t;

/**
 * @brief Si70xx device descriptor.
 */
typedef struct {
    si70xx_params_t params;     /**< Device parameters */
} si70xx_t;

/**
 * @brief   Initialize and reset the sensor.
 *
 * @param[in] dev           device descriptor
 * @param[in] params        initialization parameters
 *
 * @return                  SI70XX_OK on successful initialization
 * @return                  -SI70XX_ERR_NOI2C on I2C initialization error
 * @return                  -SI70XX_ERR_NODEV on device test error
 * @return                  -SI70XX_ERR_I2C on I2C bus error
 */
int si70xx_init(si70xx_t *dev, const si70xx_params_t *params);

/**
 * @brief   Read the relative humidity from the sensor. Uses clock streching.
 *
 * @param[in] dev           device descriptor
 * @return                  relative humidity in centi-percent (times 100)
 */
uint16_t si70xx_get_relative_humidity(const si70xx_t *dev);

/**
 * @brief   Read the current temperature from the sensor. Uses clock streching.
 *
 * @param[in] dev           device descriptor
 * @return                  current temperature in centi-degrees Celsius
 *                          (times 100)
 */
int16_t si70xx_get_temperature(const si70xx_t *dev);

/**
 * @brief   Read the relative humidity and temperature from the sensor. Uses
 *          clock stretching.
 *
 * @param[in] dev           device descriptor
 * @param[out] humidity     pointer to relative humidity (in centi-percent)
 * @param[out] temperature  pointer to temperature (in centi-degrees Celsius)
 */
void si70xx_get_both(const si70xx_t *dev, uint16_t *humidity, int16_t *temperature);

/**
 * @brief   Read the sensor serial number.
 *
 * @param[in] dev           device descriptor
 * @return                  sensor serial number
 */
uint64_t si70xx_get_serial(const si70xx_t *dev);

/**
 * @brief   Read the sensor id, to identifier the sensor variant.
 *
 * @param[in] dev           device descriptor
 * @return                  device id
 */
uint8_t si70xx_get_id(const si70xx_t *dev);

/**
 * @brief   Read the firmware revision of the sensor.
 *
 * @param[in] dev           device descriptor
 * @return                  sensor firmware revision number
 */
uint8_t si70xx_get_revision(const si70xx_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* SI70XX_H */
/** @} */
