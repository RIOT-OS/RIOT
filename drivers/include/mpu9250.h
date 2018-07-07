/*
 * Copyright (C) 2015 Freie Universität Berlin
 * Copyright 2018 Ville Hiltunen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_mpu9250 MPU-9250 accelerometer/magnetometer/gyroscope
 * @ingroup     drivers_sensors
 * @brief       Device driver interface for the MPU-9250
 * @{
 *
 * @file
 * @brief       Device driver interface for the MPU-9250
 *
 * @note        Originally the driver for MPU-9150, repurposed for MPU-9250
 *
 * @author      Fabian Nack <nack@inf.fu-berlin.de>,
 *              modified by Ville Hiltunen <hiltunenvillej@gmail.com>
 */

#ifndef MPU9250_H
#define MPU9250_H

#include "periph/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Sample rate macro definitions
 * @{
 */
#define MPU9250_MIN_SAMPLE_RATE     (4)
#define MPU9250_MAX_SAMPLE_RATE     (1000)
#define MPU9250_DEFAULT_SAMPLE_RATE (50)
#define MPU9250_MIN_COMP_SMPL_RATE  (1)
#define MPU9250_MAX_COMP_SMPL_RATE  (100)
/** @} */

/**
 * @name    Interrupt register macros
 * @{
 */
#define MPU9250_INT_EN_CFG          (0x12)
#define MPU9250_INT_EN              (0x01)

/** @} */

/**
 * @name    Power Management 1 register macros
 * @{
 */
#define MPU9250_PWR_WAKEUP          (0x00)
#define MPU9250_PWR_PLL             (0x01)
#define MPU9250_PWR_RESET           (0x80)
/** @} */

/**
 * @name    Power Management 2 register macros
 * @{
 */
#define MPU9250_PWR_GYRO            (0x07)
#define MPU9250_PWR_ACCEL           (0x38)
/** @} */

/**
 * @name    Sleep times in microseconds
 * @{
 */
#define MPU9250_COMP_MODE_SLEEP_US  (1000)
#define MPU9250_BYPASS_SLEEP_US     (3000)
#define MPU9250_PWR_CHANGE_SLEEP_US (50000)
#define MPU9250_RESET_SLEEP_US      (100000)
/** @} */

/**
 * @name    MPU-9250 compass operating modes and reg values
 * @{
 */
#define MPU9250_COMP_POWER_DOWN     (0x00)
#define MPU9250_COMP_SINGLE_MEASURE (0x01)
#define MPU9250_COMP_SELF_TEST      (0x08)
#define MPU9250_COMP_FUSE_ROM       (0x0F)
#define MPU9250_COMP_WHOAMI_ANSWER  (0x48)
/** @} */

/**
 * @brief   Power enum values
 */
typedef enum {
    MPU9250_SENSOR_PWR_OFF = 0x00,
    MPU9250_SENSOR_PWR_ON = 0x01,
} mpu9250_pwr_t;

/**
 * @brief   Possible MPU-9250 hardware addresses (wiring specific)
 */
typedef enum {
    MPU9250_HW_ADDR_HEX_68 = 0x68,
    MPU9250_HW_ADDR_HEX_69 = 0x69,
} mpu9250_hw_addr_t;

/**
 * @brief   Possible compass addresses (wiring specific)
 */
typedef enum {
    MPU9250_COMP_ADDR_HEX_0C = 0x0C,
    MPU9250_COMP_ADDR_HEX_0D = 0x0D,
    MPU9250_COMP_ADDR_HEX_0E = 0x0E,
    MPU9250_COMP_ADDR_HEX_0F = 0x0F,
} mpu9250_comp_addr_t;

/**
 * @brief   Possible full scale ranges for the gyroscope
 */
typedef enum {
    MPU9250_GYRO_FSR_250DPS = 0x00,
    MPU9250_GYRO_FSR_500DPS = 0x01,
    MPU9250_GYRO_FSR_1000DPS = 0x02,
    MPU9250_GYRO_FSR_2000DPS = 0x03,
} mpu9250_gyro_ranges_t;

/**
 * @brief   Possible full scale ranges for the accelerometer
 */
typedef enum {
    MPU9250_ACCEL_FSR_2G = 0x00,
    MPU9250_ACCEL_FSR_4G = 0x01,
    MPU9250_ACCEL_FSR_8G = 0x02,
    MPU9250_ACCEL_FSR_16G = 0x03,
} mpu9250_accel_ranges_t;

/**
 * @brief   Possible low pass filter values
 */
typedef enum {
    MPU9250_FILTER_184HZ = 0x01,
    MPU9250_FILTER_92HZ = 0x02,
    MPU9250_FILTER_41HZ = 0x03,
    MPU9250_FILTER_20HZ = 0x04,
    MPU9250_FILTER_10HZ = 0x05,
    MPU9250_FILTER_5HZ = 0x06,
} mpu9250_lpf_t;

/**
 * @brief   MPU-9250 result vector struct
 */
typedef struct {
    int16_t x_axis;             /**< X-Axis measurement result */
    int16_t y_axis;             /**< Y-Axis measurement result */
    int16_t z_axis;             /**< Z-Axis measurement result */
} mpu9250_results_t;

/**
 * @brief   Configuration struct for the MPU-9250 sensor
 */
typedef struct {
    mpu9250_pwr_t accel_pwr;            /**< Accel power status (on/off) */
    mpu9250_pwr_t gyro_pwr;             /**< Gyro power status (on/off) */
    mpu9250_pwr_t compass_pwr;          /**< Compass power status (on/off) */
    mpu9250_gyro_ranges_t gyro_fsr;     /**< Configured gyro full-scale range */
    mpu9250_accel_ranges_t accel_fsr;   /**< Configured accel full-scale range */
    uint16_t sample_rate;               /**< Configured sample rate for accel and gyro */
    uint8_t compass_sample_rate;        /**< Configured compass sample rate */
    uint8_t compass_x_adj;              /**< Compass X-Axis sensitivity adjustment value */
    uint8_t compass_y_adj;              /**< Compass Y-Axis sensitivity adjustment value */
    uint8_t compass_z_adj;              /**< Compass Z-Axis sensitivity adjustment value */
} mpu9250_status_t;

/**
 * @brief   Device initialization parameters
 */
typedef struct {
    i2c_t i2c;                  /**< I2C device which is used */
    uint8_t addr;               /**< Hardware address of the MPU-9250 */
    uint8_t comp_addr;          /**< Address of the MPU-9250s compass */
    uint16_t sample_rate;       /**< Sample rate */
} mpu9250_params_t;

/**
 * @brief   Device descriptor for the MPU-9250 sensor
 */
typedef struct {
    mpu9250_params_t params;    /**< Device initialization parameters */
    mpu9250_status_t conf;      /**< Device configuration */
} mpu9250_t;

/**
 * @brief   Initialize the given MPU9250 device
 *
 * @param[out] dev          Initialized device descriptor of MPU9250 device
 * @param[in]  params       Initialization parameters
 *
 * @return                  0 on success
 * @return                  -1 if given I2C is not enabled in board config
 */
int mpu9250_init(mpu9250_t *dev, const mpu9250_params_t *params);

/**
 * @brief   Enable or disable accelerometer power
 *
 * @param[in] dev           Device descriptor of MPU9250 device
 * @param[in] pwr_conf      Target power setting: PWR_ON or PWR_OFF
 *
 * @return                  0 on success
 * @return                  -1 if given I2C is not enabled in board config
 */
int mpu9250_set_accel_power(mpu9250_t *dev, mpu9250_pwr_t pwr_conf);

/**
 * @brief   Enable or disable gyroscope power
 *
 * @param[in] dev           Device descriptor of MPU9250 device
 * @param[in] pwr_conf      Target power setting: PWR_ON or PWR_OFF
 *
 * @return                  0 on success
 * @return                  -1 if given I2C is not enabled in board config
 */
int mpu9250_set_gyro_power(mpu9250_t *dev, mpu9250_pwr_t pwr_conf);

/**
 * @brief   Enable or disable compass power
 *
 * @param[in] dev           Device descriptor of MPU9250 device
 * @param[in] pwr_conf      Target power setting: PWR_ON or PWR_OFF
 *
 * @return                  0 on success
 * @return                  -1 if given I2C is not enabled in board config
 */
int mpu9250_set_compass_power(mpu9250_t *dev, mpu9250_pwr_t pwr_conf);

/**
 * @brief   Read angular speed values from the given MPU9250 device, returned in dps
 *
 * The raw gyroscope data is read from the sensor and normalized with respect to
 * the configured gyroscope full-scale range.
 *
 * @param[in]  dev          Device descriptor of MPU9250 device to read from
 * @param[out] output       Result vector in dps per axis
 *
 * @return                  0 on success
 * @return                  -1 if device's I2C is not enabled in board config
 * @return                  -2 if gyro full-scale range is configured wrong
 */
int mpu9250_read_gyro(const mpu9250_t *dev, mpu9250_results_t *output);

/**
 * @brief   Read acceleration values from the given MPU9250 device, returned in mG
 *
 * The raw acceleration data is read from the sensor and normalized with respect to
 * the configured accelerometer full-scale range.
 *
 * @param[in]  dev          Device descriptor of MPU9250 device to read from
 * @param[out] output       Result vector in mG per axis
 *
 * @return                  0 on success
 * @return                  -1 if device's I2C is not enabled in board config
 * @return                  -2 if accel full-scale range is configured wrong
 */
int mpu9250_read_accel(const mpu9250_t *dev, mpu9250_results_t *output);

/**
 * @brief   Read magnetic field values from the given MPU9250 device, returned in mikroT
 *
 * The raw compass data is read from the sensor and normalized with respect to
 * the compass full-scale range (which can not be configured).
 *
 * @param[in]  dev          Device descriptor of MPU9250 device to read from
 * @param[out] output       Result vector in mikroT per axis
 *
 * @return                  0 on success
 * @return                  -1 if device's I2C is not enabled in board config
 */
int mpu9250_read_compass(const mpu9250_t *dev, mpu9250_results_t *output);

/**
 * @brief   Read temperature value from the given MPU9250 device, returned in m°C
 *
 * @note
 * The measured temperature is slightly higher than the real room temperature.
 * Tests showed that the offset varied around 2-3 °C (but no warranties here).
 *
 * @param[in] dev           Device descriptor of MPU9250 device to read from
 * @param[out] output       Temperature in m°C
 *
 * @return                  0 on success
 * @return                  -1 if device's I2C is not enabled in board config
 */
int mpu9250_read_temperature(const mpu9250_t *dev, int32_t *output);

/**
 * @brief   Set the full-scale range for raw gyroscope data
 *
 * @param[in] dev           Device descriptor of MPU9250 device
 * @param[in] fsr           Target full-scale range
 *
 * @return                  0 on success
 * @return                  -1 if device's I2C is not enabled in board config
 * @return                  -2 if given full-scale target value is not valid
 */
int mpu9250_set_gyro_fsr(mpu9250_t *dev, mpu9250_gyro_ranges_t fsr);

/**
 * @brief   Set the full-scale range for raw accelerometer data
 *
 * @param[in] dev           Device descriptor of MPU9250 device
 * @param[in] fsr           Target full-scale range
 *
 * @return                  0 on success
 * @return                  -1 if device's I2C is not enabled in board config
 * @return                  -2 if given full-scale target value is not valid
 */
int mpu9250_set_accel_fsr(mpu9250_t *dev, mpu9250_accel_ranges_t fsr);

/**
 * @brief   Set the rate at which the gyroscope and accelerometer data is sampled
 *
 * Sample rate can be chosen between 4 Hz and 1kHz. The actual set value might
 * slightly differ. If necessary, check the actual set value in the device's
 * config member afterwards.
 *
 * @param[in] dev           Device descriptor of MPU9250 device
 * @param[in] rate          Target sample rate in Hz
 *
 * @return                  0 on success
 * @return                  -1 if device's I2C is not enabled in board config
 * @return                  -2 if given target sample rate is not valid
 */
int mpu9250_set_sample_rate(mpu9250_t *dev, uint16_t rate);

/**
 * @brief   Set the rate at which the compass data is sampled
 *
 * Sample rate can be chosen between 1 Hz and 100 Hz but has to be a fraction
 * of the configured accel/gyro sample rate. The actual set value might
 * slightly differ. If necessary, check the actual set value in the device's
 * config member afterwards.
 *
 * @param[in] dev           Device descriptor of MPU9250 device
 * @param[in] rate          Target sample rate in Hz
 *
 * @return                  0 on success
 * @return                  -1 if device's I2C is not enabled in board config
 * @return                  -2 if given target sample rate is not valid
 */
int mpu9250_set_compass_sample_rate(mpu9250_t *dev, uint8_t rate);

/*
 * @brief   Enable or disable mpu9250 interrupt pin.
 *
 * @param[in] dev           Device descriptor of MPU9250 device
 * @param[in] enable        1 to enable interrupts, 0 to disable
 *
 * @return                  0 on success
 * @return                  -1 on failure
 */
int mpu9250_set_interrupt(mpu9250_t *dev, uint8_t enable);

#ifdef __cplusplus
}
#endif

#endif /* MPU9250_H */
/** @} */
