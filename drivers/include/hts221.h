/*
 * SPDX-FileCopyrightText: 2017 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    drivers_hts221 ST HTS221 digital Humidity Sensor
 * @ingroup     drivers_sensors
 * @ingroup     drivers_saul
 * @brief       Driver for the ST HTS221 digital Humidity Sensor
 *
 * This driver provides @ref drivers_saul capabilities.
 *
 * @{
 * @file
 * @brief       Interface definition for the ST HTS221 driver
 *
 * @author      Sebastian Meiling <s@mlng.net>
 */

#include <stdint.h>

#include "periph/i2c.h"
#include "hts221_regs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Default I2C bus address (7 Bit) of HTS221 devices
 */
#define HTS221_I2C_ADDRESS           (0x5F)

/**
 * @brief   Parameters needed for device initialization
 */
typedef struct {
    i2c_t i2c;              /**< bus the device is connected to */
    uint8_t addr;           /**< address on that bus */
    uint8_t avgx;           /**< average sampling of humidity and temperature */
    uint8_t rate;           /**< output data rate */
} hts221_params_t;

/**
 * @brief Return values and error codes
 */
enum {
    HTS221_OK = 0,
    HTS221_ERROR,
    HTS221_NOBUS,
    HTS221_NODEV,
};

/**
 * @brief   Device descriptor for HTS221 sensors
 */
typedef struct {
    hts221_params_t p;      /**< Configuration parameters */
    int16_t h0_rh;          /**< lower humitidy reference */
    int16_t h1_rh;          /**< upper humitidy reference */
    int16_t h0_t0_out;      /**< lower humitidy to temperature reference */
    int16_t h1_t0_out;      /**< upper humitidy to temperature reference */
    int16_t t0_degc;        /**< lower temperature reference in degC */
    int16_t t1_degc;        /**< upper temperature reference in degC */
    int16_t t0_out;         /**< lower temperature reference raw value */
    int16_t t1_out;         /**< upper temperature reference raw value */
} hts221_t;

/**
 * @brief   Initialize the given HTS221 device
 *
 * @param[out] dev          device descriptor of sensor to initialize
 * @param[in]  params       configuration parameters
 *
 * @return                  HTS221_OK on success
 * @return                  HTS221_NOBUS if initialization of I2C bus fails
 * @return                  HTS221_NODEV if no HTS221 device found on bus
 */
int hts221_init(hts221_t *dev, const hts221_params_t *params);

/**
 * @brief   Reboot device and reload base configuration
 *
 * @param[in]  dev          device descriptor of sensor
 *
 * @return                  0 on success, or error otherwise
 */
int hts221_reboot(const hts221_t *dev);

/**
 * @brief   Set device to one shot measurement
 *
 * @param[in]  dev          device descriptor of sensor
 *
 * @return                  0 on success, or error otherwise
 */
int hts221_one_shot(const hts221_t *dev);

/**
 * @brief   Set device to continuous measurements
 *
 * @param[in]  dev          device descriptor of sensor
 * @param[in]  rate         conversion rate for continuous mode
 *
 * @return                  0 on success, or error otherwise
 */
int hts221_set_rate(const hts221_t *dev, const uint8_t rate);

/**
 * @brief   Set device to active
 *
 * @param[in]  dev          device descriptor of sensor
 *
 * @return                  0 on success, or error otherwise
 */
int hts221_power_on(const hts221_t *dev);

/**
 * @brief   Set device to power down
 *
 * @param[in]  dev          device descriptor of sensor
 *
 * @return                  0 on success, or error otherwise
 */
int hts221_power_off(const hts221_t *dev);

/**
 * @brief   Set device to power down
 *
 * @param[in]  dev          device descriptor of sensor
 *
 * @return                  >=0 on success
 * @return                  -HTS221_NOBUS on error
 */
int hts221_get_state(const hts221_t *dev);

/**
 * @brief   Reading humidity and temperature
 *
 * @param[in]  dev          device descriptor of sensor
 * @param[out] val          humidity [in 10 * percent relative]
 *
 * @return                  0 on success, or error otherwise
 */
int hts221_read_humidity(const hts221_t *dev, uint16_t *val);

/**
 * @brief   Reading humidity and temperature
 *
 * @param[in]  dev          device descriptor of sensor
 * @param[out] val          temperature [in 100 * degree centigrade]
 *
 * @return                  0 on success, or error otherwise
 */
int hts221_read_temperature(const hts221_t *dev, int16_t *val);

#ifdef __cplusplus
}
#endif

/** @} */
