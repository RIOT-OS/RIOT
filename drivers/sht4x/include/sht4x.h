/*
 * SPDX-FileCopyrightText: 2026 RIOT Contributors
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_sht4x
 * @brief       Device driver for Sensirion SHT4X Temperature and Humidity Sensors
 * @file
 * @{
 */

#include <stdint.h>
#include <stdbool.h>

#include "periph/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

/** possible I2C slave addresses */
#define SHT4X_I2C_ADDR_1    (0x44)
#define SHT4X_I2C_ADDR_2    (0x45)

/** @brief Driver error codes (returned as negative values) */
enum {
    SHT4X_OK,                       /**< success */
    SHT4X_ERROR_I2C,                /**< I2C bus error */
    SHT4X_ERROR_CRC,                /**< checksum mismatch on received data */
};

/** @brief SHT4x repeatability levels, higher repeatability takes longer
 *         but reduces measurement noise */
typedef enum {
    SHT4X_LOW = 0,  /**< low repeatability, fastest measurement */
    SHT4X_MEDIUM,   /**< medium repeatability */
    SHT4X_HIGH      /**< high repeatability, slowest measurement */
} sht4x_repeat_t;

/** @brief SHT4x heater power levels, used with @ref sht4x_set_heater_power */
typedef enum {
    SHT4X_HEATER_POWER_HIGH = 0,   /**< highest heater power */
    SHT4X_HEATER_POWER_MEDIUM,     /**< medium heater power */
    SHT4X_HEATER_POWER_LOW         /**< lowest heater power */
} sht4x_heater_power_t;

/** @brief SHT4x heater durations, used with @ref sht4x_set_heater_duration */
typedef enum {
    SHT4X_HEATER_DURATION_LONG = 0,    /**< ~1 s heating pulse */
    SHT4X_HEATER_DURATION_SHORT        /**< ~0.1 s heating pulse */
} sht4x_heater_duration_t;

/** @brief SHT4x device initialization parameters */
typedef struct {
    i2c_t          i2c_dev;    /**< I2C bus the sensor is connected to */
    uint8_t        i2c_addr;   /**< I2C slave address, e.g. @ref SHT4X_I2C_ADDR_1 */
    sht4x_repeat_t repeat;     /**< measurement repeatability */
} sht4x_params_t;

/** @brief SHT4x sensor device descriptor */
typedef struct {
    i2c_t                    i2c_dev;         /**< I2C bus the sensor is connected to */
    uint8_t                  i2c_addr;        /**< I2C slave address */
    sht4x_repeat_t           repeat;          /**< measurement repeatability */
    sht4x_heater_power_t     heater_power;    /**< heater power, set with @ref sht4x_set_heater_power */
    sht4x_heater_duration_t  heater_duration; /**< heater duration, set with @ref sht4x_set_heater_duration */
} sht4x_dev_t;

/**
 * @brief   Initialize the SHT4x sensor
 *
 * @param[in]  dev      device descriptor to initialize
 * @param[in]  params   initialization parameters
 *
 * @retval  SHT4X_OK            success
 * @retval  -SHT4X_ERROR_I2C    could not reset the sensor
 */
int sht4x_init(sht4x_dev_t *dev, const sht4x_params_t *params);

/**
 * @brief   Measure temperature and/or relative humidity
 *
 * The sensor always measures both temperature and humidity in a single I2C
 * transaction, so both are returned together; pass NULL for a value that is
 * not needed.
 *
 * @param[in]  dev      device descriptor
 * @param[out] temp     temperature in centi °C (accuracy ±0.1 °C), may be NULL
 * @param[out] hum      relative humidity in whole percent (accuracy ±1 %RH),
 *                       may be NULL
 *
 * @retval  SHT4X_OK            success
 * @retval  -SHT4X_ERROR_I2C    I2C bus error
 * @retval  -SHT4X_ERROR_CRC    checksum mismatch on received data
 */
int sht4x_read(sht4x_dev_t *dev, int16_t *temp, int16_t *hum);

/**
 * @brief   Measure temperature and/or relative humidity with the heater on
 *
 * Uses the heater power and duration previously set with
 * @ref sht4x_set_heater_power and @ref sht4x_set_heater_duration.
 *
 * @param[in]  dev      device descriptor
 * @param[out] temp     temperature in centi °C (accuracy ±0.1 °C), may be NULL
 * @param[out] hum      relative humidity in whole percent (accuracy ±1 %RH),
 *                       may be NULL
 *
 * @retval  SHT4X_OK            success
 * @retval  -SHT4X_ERROR_I2C    I2C bus error
 * @retval  -SHT4X_ERROR_CRC    checksum mismatch on received data
 */
int sht4x_read_with_heater(sht4x_dev_t *dev, int16_t *temp, int16_t *hum);

/**
 * @brief   Set the heater power used by @ref sht4x_read_with_heater
 *
 * @param[in]  dev      device descriptor
 * @param[in]  power    heater power level
 *
 * @retval  SHT4X_OK    success
 */
int sht4x_set_heater_power(sht4x_dev_t *dev, sht4x_heater_power_t power);

/**
 * @brief   Set the heater duration used by @ref sht4x_read_with_heater
 *
 * @param[in]  dev        device descriptor
 * @param[in]  duration   heater duration
 *
 * @retval  SHT4X_OK    success
 */
int sht4x_set_heater_duration(sht4x_dev_t *dev, sht4x_heater_duration_t duration);

#ifdef __cplusplus
}
#endif

/** @} */
