/*
 * Copyright (C) 2015 Freie Universität Berlin
 *               2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    drivers_mpu9x50 MPU-9X50 (MPU9150 and MPU9250) accelerometer/magnetometer/gyroscope
 * @ingroup     drivers_sensors
 * @ingroup     drivers_saul
 * @brief       Device driver interface for the MPU-9X50 (MPU9150 and MPU9250)
 *
 * This driver provides @ref drivers_saul capabilities.
 * @{
 *
 * @file
 * @brief       Device driver interface for the MPU-9X50 (MPU9150 and MPU9250)
 *
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 * @author      Jannes Volkens <jannes.volkens@haw-hamburg.de>
 */

#include "periph/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Sample rate macro definitions
 * @{
 */
#define MPU9X50_MIN_SAMPLE_RATE     (4)
#define MPU9X50_MAX_SAMPLE_RATE     (1000)
#define MPU9X50_DEFAULT_SAMPLE_RATE (50)
#define MPU9X50_MIN_COMP_SMPL_RATE  (1)
#define MPU9X50_MAX_COMP_SMPL_RATE  (100)
/** @} */

/**
 * @brief   Power enum values
 */
typedef enum {
    MPU9X50_SENSOR_PWR_OFF = 0x00,
    MPU9X50_SENSOR_PWR_ON = 0x01,
} mpu9x50_pwr_t;

/**
 * @brief   Possible MPU-9X50 hardware addresses (wiring specific)
 */
typedef enum {
    MPU9X50_HW_ADDR_HEX_68 = 0x68,
    MPU9X50_HW_ADDR_HEX_69 = 0x69,
} mpu9x50_hw_addr_t;

/**
 * @brief   Possible compass addresses (wiring specific)
 */
typedef enum {
    MPU9X50_COMP_ADDR_HEX_0C = 0x0C,
    MPU9X50_COMP_ADDR_HEX_0D = 0x0D,
    MPU9X50_COMP_ADDR_HEX_0E = 0x0E,
    MPU9X50_COMP_ADDR_HEX_0F = 0x0F,
} mpu9x50_comp_addr_t;

/**
 * @brief   Possible full scale ranges for the gyroscope
 */
typedef enum {
    MPU9X50_GYRO_FSR_250DPS = 0x00,
    MPU9X50_GYRO_FSR_500DPS = 0x01,
    MPU9X50_GYRO_FSR_1000DPS = 0x02,
    MPU9X50_GYRO_FSR_2000DPS = 0x03,
} mpu9x50_gyro_ranges_t;

/**
 * @brief   Possible full scale ranges for the accelerometer
 */
typedef enum {
    MPU9X50_ACCEL_FSR_2G = 0x00,
    MPU9X50_ACCEL_FSR_4G = 0x01,
    MPU9X50_ACCEL_FSR_8G = 0x02,
    MPU9X50_ACCEL_FSR_16G = 0x03,
} mpu9x50_accel_ranges_t;

/**
 * @brief   Possible low pass filter values
 */
typedef enum {
    MPU9X50_FILTER_188HZ = 0x01,
    MPU9X50_FILTER_98HZ = 0x02,
    MPU9X50_FILTER_42HZ = 0x03,
    MPU9X50_FILTER_20HZ = 0x04,
    MPU9X50_FILTER_10HZ = 0x05,
    MPU9X50_FILTER_5HZ = 0x06,
} mpu9x50_lpf_t;

/**
 * @brief   MPU-9X50 result vector struct
 */
typedef struct {
    int16_t x_axis;             /**< X-Axis measurement result */
    int16_t y_axis;             /**< Y-Axis measurement result */
    int16_t z_axis;             /**< Z-Axis measurement result */
} mpu9x50_results_t;

/**
 * @brief   Configuration struct for the MPU-9X50 sensor
 */
typedef struct {
    mpu9x50_pwr_t accel_pwr;            /**< Accel power status (on/off) */
    mpu9x50_pwr_t gyro_pwr;             /**< Gyro power status (on/off) */
    mpu9x50_pwr_t compass_pwr;          /**< Compass power status (on/off) */
    mpu9x50_gyro_ranges_t gyro_fsr;     /**< Configured gyro full-scale range */
    mpu9x50_accel_ranges_t accel_fsr;   /**< Configured accel full-scale range */
    uint16_t sample_rate;               /**< Configured sample rate for accel and gyro */
    uint8_t compass_sample_rate;        /**< Configured compass sample rate */
    uint8_t compass_x_adj;              /**< Compass X-Axis sensitivity adjustment value */
    uint8_t compass_y_adj;              /**< Compass Y-Axis sensitivity adjustment value */
    uint8_t compass_z_adj;              /**< Compass Z-Axis sensitivity adjustment value */
} mpu9x50_status_t;

/**
 * @brief   Device initialization parameters
 */
typedef struct {
    i2c_t i2c;                  /**< I2C device which is used */
    uint8_t addr;               /**< Hardware address of the MPU-9X50 */
    uint8_t comp_addr;          /**< Address of the MPU-9X50s compass */
    uint16_t sample_rate;       /**< Sample rate */
} mpu9x50_params_t;

/**
 * @brief   Device descriptor for the MPU9X50 sensor
 */
typedef struct {
    mpu9x50_params_t params;    /**< Device initialization parameters */
    mpu9x50_status_t conf;      /**< Device configuration */
} mpu9x50_t;

/**
 * @brief   Initialize the given MPU9X50 device
 *
 * @param[out] dev          Initialized device descriptor of MPU9X50 device
 * @param[in]  params       Initialization parameters
 *
 * @return                  0 on success
 * @return                  -1 if given I2C is not enabled in board config
 */
int mpu9x50_init(mpu9x50_t *dev, const mpu9x50_params_t *params);

/**
 * @brief   Enable or disable accelerometer power
 *
 * @param[in] dev           Device descriptor of MPU9X50 device
 * @param[in] pwr_conf      Target power setting: PWR_ON or PWR_OFF
 *
 * @return                  0 on success
 * @return                  -1 if given I2C is not enabled in board config
 */
int mpu9x50_set_accel_power(mpu9x50_t *dev, mpu9x50_pwr_t pwr_conf);

/**
 * @brief   Enable or disable gyroscope power
 *
 * @param[in] dev           Device descriptor of MPU9X50 device
 * @param[in] pwr_conf      Target power setting: PWR_ON or PWR_OFF
 *
 * @return                  0 on success
 * @return                  -1 if given I2C is not enabled in board config
 */
int mpu9x50_set_gyro_power(mpu9x50_t *dev, mpu9x50_pwr_t pwr_conf);

/**
 * @brief   Enable or disable compass power
 *
 * @param[in] dev           Device descriptor of MPU9X50 device
 * @param[in] pwr_conf      Target power setting: PWR_ON or PWR_OFF
 *
 * @return                  0 on success
 * @return                  -1 if given I2C is not enabled in board config
 */
int mpu9x50_set_compass_power(mpu9x50_t *dev, mpu9x50_pwr_t pwr_conf);

/**
 * @brief   Read angular speed values from the given MPU9X50 device, returned in dps
 *
 * The raw gyroscope data is read from the sensor and normalized with respect to
 * the configured gyroscope full-scale range.
 *
 * @param[in]  dev          Device descriptor of MPU9X50 device to read from
 * @param[out] output       Result vector in dps per axis
 *
 * @return                  0 on success
 * @return                  -1 if device's I2C is not enabled in board config
 * @return                  -2 if gyro full-scale range is configured wrong
 */
int mpu9x50_read_gyro(const mpu9x50_t *dev, mpu9x50_results_t *output);

/**
 * @brief   Read acceleration values from the given MPU9X50 device, returned in mG
 *
 * The raw acceleration data is read from the sensor and normalized with respect to
 * the configured accelerometer full-scale range.
 *
 * @param[in]  dev          Device descriptor of MPU9X50 device to read from
 * @param[out] output       Result vector in mG per axis
 *
 * @return                  0 on success
 * @return                  -1 if device's I2C is not enabled in board config
 * @return                  -2 if accel full-scale range is configured wrong
 */
int mpu9x50_read_accel(const mpu9x50_t *dev, mpu9x50_results_t *output);

/**
 * @brief   Read magnetic field values from the given MPU9X50 device, returned in mikroT
 *
 * The raw compass data is read from the sensor and normalized with respect to
 * the compass full-scale range (which can not be configured).
 *
 * @param[in]  dev          Device descriptor of MPU9X50 device to read from
 * @param[out] output       Result vector in mikroT per axis
 *
 * @return                  0 on success
 * @return                  -1 if device's I2C is not enabled in board config
 */
int mpu9x50_read_compass(const mpu9x50_t *dev, mpu9x50_results_t *output);

/**
 * @brief   Read temperature value from the given MPU9X50 device, returned in m°C
 *
 * @note
 * The measured temperature is slightly higher than the real room temperature.
 * Tests showed that the offset varied around 2-3 °C (but no warranties here).
 *
 * @param[in] dev           Device descriptor of MPU9X50 device to read from
 * @param[out] output       Temperature in m°C
 *
 * @return                  0 on success
 * @return                  -1 if device's I2C is not enabled in board config
 */
int mpu9x50_read_temperature(const mpu9x50_t *dev, int32_t *output);

/**
 * @brief   Set the full-scale range for raw gyroscope data
 *
 * @param[in] dev           Device descriptor of MPU9X50 device
 * @param[in] fsr           Target full-scale range
 *
 * @return                  0 on success
 * @return                  -1 if device's I2C is not enabled in board config
 * @return                  -2 if given full-scale target value is not valid
 */
int mpu9x50_set_gyro_fsr(mpu9x50_t *dev, mpu9x50_gyro_ranges_t fsr);

/**
 * @brief   Set the full-scale range for raw accelerometer data
 *
 * @param[in] dev           Device descriptor of MPU9X50 device
 * @param[in] fsr           Target full-scale range
 *
 * @return                  0 on success
 * @return                  -1 if device's I2C is not enabled in board config
 * @return                  -2 if given full-scale target value is not valid
 */
int mpu9x50_set_accel_fsr(mpu9x50_t *dev, mpu9x50_accel_ranges_t fsr);

/**
 * @brief   Set the rate at which the gyroscope and accelerometer data is sampled
 *
 * Sample rate can be chosen between 4 Hz and 1kHz. The actual set value might
 * slightly differ. If necessary, check the actual set value in the device's
 * config member afterwards.
 *
 * @param[in] dev           Device descriptor of MPU9X50 device
 * @param[in] rate          Target sample rate in Hz
 *
 * @return                  0 on success
 * @return                  -1 if device's I2C is not enabled in board config
 * @return                  -2 if given target sample rate is not valid
 */
int mpu9x50_set_sample_rate(mpu9x50_t *dev, uint16_t rate);

/**
 * @brief   Set the rate at which the compass data is sampled
 *
 * Sample rate can be chosen between 1 Hz and 100 Hz but has to be a fraction
 * of the configured accel/gyro sample rate. The actual set value might
 * slightly differ. If necessary, check the actual set value in the device's
 * config member afterwards.
 *
 * @param[in] dev           Device descriptor of MPU9X50 device
 * @param[in] rate          Target sample rate in Hz
 *
 * @return                  0 on success
 * @return                  -1 if device's I2C is not enabled in board config
 * @return                  -2 if given target sample rate is not valid
 */
int mpu9x50_set_compass_sample_rate(mpu9x50_t *dev, uint8_t rate);

#ifdef __cplusplus
}
#endif

/** @} */
