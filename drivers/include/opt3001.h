/*
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_opt3001 OPT3001 Ambient Light Sensor
 * @ingroup     drivers_sensors
 * @ingroup     drivers_saul
 * @brief       Driver for the Texas Instruments OPT3001 sensor.
 *
 * The TI OPT3001 (Ambient Light Sensor) measures
 * the intensity of visible light with a spectal response comparable to the human eye,
 * with simultaneous rejection of infrared light.
 * The conversion time, mode of conversion and the interrupt reporting mechanism mode can be set
 * by the sensor initialization.
 * After initialization the raw sensor values can be read.
 * The ambient light can be calculated as follows:<br><br>
 * Result register translation into lux:
 *     \f{eqnarray*}{
 *         Lux &=& LSB\_Size \cdot R[11:0]
 *     \f}
 * Where LSB\_Size:
 *     \f{eqnarray*}{
 *         LSB\_Size &=& 0.01 \cdot 2^{E[3:0]}
 *     \f}
 *
 * The calculation is wrapped from TI OPT3001 Ambient Light Sensor (ALS) datasheet (Rev. C).
 *
 * This driver provides @ref drivers_saul capabilities.
 * @{
 *
 * @file
 * @brief       Interface definition for the OPT3001 sensor driver.
 *
 * @author      Jannes Volkens <jannes.volkens@haw-hamburg.de>
 */

#ifndef OPT3001_H
#define OPT3001_H

#include <stdint.h>
#include "periph/i2c.h"
#include "kernel_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Conversion time
 * @{
 */
#define OPT3001_CONVERSION_TIME_100_MS   (0x0000) /**< Conversion time of 100ms */
#define OPT3001_CONVERSION_TIME_800_MS   (0x0800) /**< Conversion time of 800ms */
/** @} */

/**
 * @defgroup drivers_opt3001_config    OPT3001 Ambient Light Sensor driver compile configuration
 * @ingroup config_drivers_sensors
 * @{
 */
/**
 * @brief   OPT3001 Default Address
 *
 * If set to 0x45 the ADDR PIN should be connected to VDD.
 * For more information on SerialBus Address, refer section 7.3.4.1 in datasheet.
 */
#ifndef CONFIG_OPT3001_I2C_ADDRESS
#define CONFIG_OPT3001_I2C_ADDRESS   (0x45)
#endif

/**
 * @brief   OPT3001 Default conversion time
 *
 * If set to 0x0000, the conversion time will be 100ms.
 * If set to 0x0800, the conversion time will be 800ms
 */
#if IS_ACTIVE(CONFIG_OPT3001_CONVERSION_TIME_100)
#define CONFIG_OPT3001_CONVERSION_TIME   OPT3001_CONVERSION_TIME_100_MS
#elif IS_ACTIVE(CONFIG_OPT3001_CONVERSION_TIME_800)
#define CONFIG_OPT3001_CONVERSION_TIME   OPT3001_CONVERSION_TIME_800_MS
#endif

#ifndef CONFIG_OPT3001_CONVERSION_TIME
#define CONFIG_OPT3001_CONVERSION_TIME   OPT3001_CONVERSION_TIME_800_MS
#endif
/** @} */

/**
 * @brief   Parameters needed for device initialization
 */
typedef struct {
    i2c_t i2c_dev; /**< I2C device, the sensor is connected to */
    uint8_t i2c_addr; /**< The sensor's slave address on the I2C bus */
} opt3001_params_t;

/**
 * @brief   Device descriptor for OPT3001 sensors
 */
typedef struct {
    opt3001_params_t params; /**< Configuration parameters */
} opt3001_t;

/**
 * @brief   OPT3001 specific return values
 */
enum {
    OPT3001_OK,         /**< Success, no error */
    OPT3001_ERROR_BUS,  /**< I2C bus error */
    OPT3001_ERROR_DEV,  /**< Internal device error */
    OPT3001_ERROR       /**< General error */
};

/**
 * @brief   Initialize the OPT3001 sensor driver.
 *
 * @param[out] dev          device descriptor of sensor to initialize
 * @param[in]  params       configuration parameters
 *
 * @return                  0 on success
 * @return                  -OPT3001_ERROR_BUS on I2C bus error
 * @return                  -OPT3001_ERROR_DEV if sensor test failed
 */
int opt3001_init(opt3001_t *dev, const opt3001_params_t *params);

/**
 * @brief   Reset the OPT3001 sensor while simultaneous deactivating measurements.
 *          Afterwards the sensor should be idle.
 *
 * @param[out] dev          device descriptor of sensor
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int opt3001_reset(const opt3001_t *dev);

/**
 * @brief   Set active mode, this enables periodic measurements.
 *
 * @param[in]  dev          device descriptor of sensor
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int opt3001_set_active(const opt3001_t *dev);

/**
 * @brief   Read sensor's raw data and convert it to milliLux.
 *
 * @param[in]  dev          device descriptor of sensor
 * @param[out] convl        ambient light in milliLux
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int opt3001_read_lux(const opt3001_t *dev, uint32_t *convl);

#ifdef __cplusplus
}
#endif

#endif /* OPT3001_H */
/** @} */
