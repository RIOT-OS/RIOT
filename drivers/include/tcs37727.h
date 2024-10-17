/*
 * Copyright (C) 2015 PHYTEC Messtechnik GmbH
 *               2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_tcs37727 TCS37727 RGB Light Sensor
 * @ingroup     drivers_sensors
 * @ingroup     drivers_saul
 * @brief       Driver for the AMS TCS37727 Color Light-To-Digital Converter
 *
 * This driver provides @ref drivers_saul capabilities.
 * @{
 *
 * @file
 * @brief       Interface definition for the TCS37727 sensor driver.
 *
 * @author      Felix Siebel <f.siebel@phytec.de>
 * @author      Johann Fischer <j.fischer@phytec.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef TCS37727_H
#define TCS37727_H

#include <stdint.h>

#include "periph/i2c.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief   Default Device Address
 */
#define TCS37727_I2C_ADDRESS    0x29

/**
 * @defgroup drivers_tcs37727_config     TCS37727 RGB Light Sensor driver compile configuration
 * @ingroup config_drivers_sensors
 * @{
 */
/**
 * @brief   Default RGBC integration time in microseconds.
 *
 * RGBC integration time impacts both the resolution and the sensitivity of the
 * RGBC reading. Refer to the section "RGBC Time Register" in the datasheet for
 * more information.
 */
#ifndef CONFIG_TCS37727_ATIME_DEFAULT
#define CONFIG_TCS37727_ATIME_DEFAULT  200000
#endif
/** @} */

/**
 * @brief   Struct for storing TCS37727 sensor data
 */
typedef struct {
    uint32_t red;           /**< IR compensated channels red */
    uint32_t green;         /**< IR compensated channels green */
    uint32_t blue;          /**< IR compensated channels blue */
    uint32_t clear;         /**< channels clear */
    uint32_t lux;           /**< Lux */
    uint32_t ct;            /**< Color temperature */
} tcs37727_data_t;

/**
 * @brief   TCS37727 configuration parameters
 */
typedef struct {
    i2c_t i2c;              /**< I2C bus the sensor is connected to */
    uint8_t addr;           /**< the sensors address on the I2C bus */
    uint32_t atime;         /**< conversion time in microseconds */
} tcs37727_params_t;

/**
 * @brief   Device descriptor for TCS37727 sensors
 */
typedef struct {
    tcs37727_params_t p;    /**< device configuration */
    int again;              /**< amount of gain */
} tcs37727_t;

/**
 * @brief   Possible TCS27737 return values
 */
enum {
    TCS37727_OK     =  0,   /**< everything worked as expected */
    TCS37727_NOBUS  = -1,   /**< access to the configured I2C bus failed */
    TCS37727_NODEV  = -2    /**< no TCS37727 device found on the bus */
};

/**
 * @brief   Initialize the given TCS37727 sensor
 *
 * The sensor is initialized in RGBC only mode with proximity detection turned
 * off.
 *
 * The gain will be initially set to 4x, but it will be adjusted
 *
 * The gain value will be initially set to 4x, but it will be automatically
 * adjusted during runtime.
 *
 * @param[out] dev          device descriptor of sensor to initialize
 * @param[in]  params       static configuration parameters
 *
 * @return                  TCS27737_OK on success
 * @return                  TCS37727_NOBUS if initialization of I2C bus fails
 * @return                  TCS37727_NODEV if no sensor can be found
 */
int tcs37727_init(tcs37727_t *dev, const tcs37727_params_t *params);

/**
 * @brief   Set RGBC enable, this activates periodic RGBC measurements.
 *
 * @param[out] dev          device descriptor of sensor
 */
void tcs37727_set_rgbc_active(const tcs37727_t *dev);

/**
 * @brief   Set RGBC disable, this deactivates periodic RGBC measurements
 *
 * Also turns off the sensor when proximity measurement is disabled.
 *
 * @param[in]  dev          device descriptor of sensor
 */
void tcs37727_set_rgbc_standby(const tcs37727_t *dev);

/**
 * @brief   Read sensor's data
 *
 * Besides an Autogain routine is called. If a maximum or minimum threshold
 * value of the channel clear is reached, then the gain will be changed
 * correspond to max or min threshold.
 *
 * @param[in]  dev         device descriptor of sensor
 * @param[out] data        device sensor data, MUST not be NULL
 */
void tcs37727_read(const tcs37727_t *dev, tcs37727_data_t *data);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* TCS37727_H */
