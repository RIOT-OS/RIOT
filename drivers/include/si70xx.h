/*
 * SPDX-FileCopyrightText: 2016 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    drivers_si70xx Si7006/13/20/21/5x temperature and humidity sensors
 * @ingroup     drivers_sensors
 * @ingroup     drivers_saul
 * @brief       Driver for the Si7006/13/20/21/5x temperature and humidity sensor
 *
 * The Si70xx driver supports a range of similar temperature and humidity
 * sensors from Silicon Labs.
 *
 * The Si7006/13/20/21 sensors support both temperature and relative humidity
 * reading, while the Si7050/1/3/4/5 sensors only provide a temperature reading
 * varying in accuracy between +/- 0.1 C to +/- 1.0 C depending on the model.
 *
 * For any of the Si705x models, use the pseudo module @p si705x.
 *
 * This driver provides @ref drivers_saul capabilities.
 * @{
 *
 * @file
 * @brief       Interface definition of the Si70xx driver.
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 */

#include "periph/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compile time macro telling whether the chip has a humidity sensor.
 */
#if MODULE_SI7050 || MODULE_SI7051 || MODULE_SI7053 || MODULE_SI7054 || \
    MODULE_SI7055
/* Si705x sensors don't have a humidity sensor hardware, only temperature. */
#define SI70XX_HAS_HUMIDITY_SENSOR 0
#else
#define SI70XX_HAS_HUMIDITY_SENSOR 1
#endif

/**
 * @brief Driver return codes
 */
enum {
    SI70XX_OK,                 /**< All OK */
    SI70XX_ERR_NODEV,          /**< No valid device found on I2C bus */
    SI70XX_ERR_I2C,            /**< An error occurred when reading/writing on I2C bus */
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

#if SI70XX_HAS_HUMIDITY_SENSOR || DOXYGEN
/**
 * @brief   Read the relative humidity from the sensor. Uses clock stretching.
 *
 * This function is only available in models that have a humidity sensor.
 *
 * @param[in] dev           device descriptor
 * @return                  relative humidity in centi-percent (times 100)
 */
uint16_t si70xx_get_relative_humidity(const si70xx_t *dev);
#endif /* SI70XX_HAS_HUMIDITY_SENSOR || DOXYGEN */

/**
 * @brief   Read the current temperature from the sensor. Uses clock stretching.
 *
 * @param[in] dev           device descriptor
 * @return                  current temperature in centi-degrees Celsius
 *                          (times 100)
 */
int16_t si70xx_get_temperature(const si70xx_t *dev);

#if SI70XX_HAS_HUMIDITY_SENSOR || DOXYGEN
/**
 * @brief   Read the relative humidity and temperature from the sensor. Uses
 *          clock stretching.
 *
 * This function is only available in models that have a humidity sensor.
 *
 * @param[in] dev           device descriptor
 * @param[out] humidity     pointer to relative humidity (in centi-percent)
 * @param[out] temperature  pointer to temperature (in centi-degrees Celsius)
 */
void si70xx_get_both(const si70xx_t *dev, uint16_t *humidity, int16_t *temperature);
#endif /* SI70XX_HAS_HUMIDITY_SENSOR || DOXYGEN */

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

/** @} */
