/*
 * Copyright (C) 2016 RIOT-OS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_mpu MPU common
 * @ingroup     drivers_sensors
 * @brief       Device driver interface for MPU motion sensor
 * @{
 *
 * @file
 * @brief       Device driver interface for MPU motion sensors
 *
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 * @author      smlng <s@mlng.net>
 */

#ifndef MPU_COMMON_H_
#define MPU_COMMON_H_

#include "periph/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Sample rate macro definitions
 * @{
 */
#define MPU_MIN_SAMPLE_RATE     (4)
#define MPU_MAX_SAMPLE_RATE     (1000)
#define MPU_DEFAULT_SAMPLE_RATE (50)
#define MPU_MIN_COMP_SMPL_RATE  (1)
#define MPU_MAX_COMP_SMPL_RATE  (100)
/** @} */

/**
 * @name Power Management 1 register macros
 * @{
 */
#define MPU_PWR_WAKEUP          (0x00)
#define MPU_PWR_PLL             (0x01)
#define MPU_PWR_RESET           (0x80)
/** @} */

/**
 * @name Power Management 2 register macros
 * @{
 */
#define MPU_PWR_GYRO            (0x07)
#define MPU_PWR_ACCEL           (0x38)
/** @} */

/**
 * @name Sleep times in microseconds
 * @{
 */
#define MPU_COMP_MODE_SLEEP_US  (1000)
#define MPU_BYPASS_SLEEP_US     (3000)
#define MPU_PWR_CHANGE_SLEEP_US (50000)
#define MPU_RESET_SLEEP_US      (100000)
/** @} */

#ifdef MPU_WITH_COMPASS
/**
 * @name MPU- compass operating modes and reg values
 * @{
 */
#define MPU_COMP_POWER_DOWN     (0x00)
#define MPU_COMP_SINGLE_MEASURE (0x01)
#define MPU_COMP_SELF_TEST      (0x08)
#define MPU_COMP_FUSE_ROM       (0x0F)
#define MPU_COMP_WHOAMI_ANSWER  (0x48)
/** @} */
#endif

/**
 * @brief Power enum values
 */
typedef enum {
    MPU_SENSOR_PWR_OFF     = 0x00,
    MPU_SENSOR_PWR_ON      = 0x01,
} mpu_pwr_t;

/**
 * @brief Possible MPU- hardware addresses (wiring specific)
 */
typedef enum {
    MPU_HW_ADDR_HEX_68     = 0x68,
    MPU_HW_ADDR_HEX_69     = 0x69,
} mpu_hw_addr_t;

#ifdef MPU_WITH_COMPASS
/**
 * @brief Possible compass addresses (wiring specific)
 */
typedef enum {
    MPU_COMP_ADDR_HEX_0C   = 0x0C,
    MPU_COMP_ADDR_HEX_0D   = 0x0D,
    MPU_COMP_ADDR_HEX_0E   = 0x0E,
    MPU_COMP_ADDR_HEX_0F   = 0x0F,
} mpu_comp_addr_t;
#endif
/**
 * @brief Possible full scale ranges for the gyroscope
 */
typedef enum {
    MPU_GYRO_FSR_250DPS    = 0x00,
    MPU_GYRO_FSR_500DPS    = 0x01,
    MPU_GYRO_FSR_1000DPS   = 0x02,
    MPU_GYRO_FSR_2000DPS   = 0x03,
} mpu_gyro_ranges_t;

/**
 * @brief Possible full scale ranges for the accelerometer
 */
typedef enum {
    MPU_ACCEL_FSR_2G       = 0x00,
    MPU_ACCEL_FSR_4G       = 0x01,
    MPU_ACCEL_FSR_8G       = 0x02,
    MPU_ACCEL_FSR_16G      = 0x03,
} mpu_accel_ranges_t;

/**
 * @brief Possible low pass filter values
 */
typedef enum {
    MPU_FILTER_188HZ       = 0x01,
    MPU_FILTER_98HZ        = 0x02,
    MPU_FILTER_42HZ        = 0x03,
    MPU_FILTER_20HZ        = 0x04,
    MPU_FILTER_10HZ        = 0x05,
    MPU_FILTER_5HZ         = 0x06,
} mpu_lpf_t;

/**
 * @brief MPU- result vector struct
 */
typedef struct {
    int16_t x_axis;                 /**< X-Axis measurement result */
    int16_t y_axis;                 /**< Y-Axis measurement result */
    int16_t z_axis;                 /**< Z-Axis measurement result */
} mpu_results_t;

/**
 * @brief Configuration struct for the MPU- sensor
 */
typedef struct {
    mpu_pwr_t accel_pwr;            /**< Accel power status (on/off) */
    mpu_pwr_t gyro_pwr;             /**< Gyro power status (on/off) */
    mpu_gyro_ranges_t gyro_fsr;     /**< Configured gyro full-scale range */
    mpu_accel_ranges_t accel_fsr;   /**< Configured accel full-scale range */
    uint16_t sample_rate;           /**< Configured sample rate for accel and gyro */
#ifdef MPU_WITH_COMPASS
    mpu_pwr_t compass_pwr;          /**< Compass power status (on/off) */
    uint8_t compass_sample_rate;    /**< Configured compass sample rate */
    uint8_t compass_x_adj;          /**< Compass X-Axis sensitivity adjustment value */
    uint8_t compass_y_adj;          /**< Compass Y-Axis sensitivity adjustment value */
    uint8_t compass_z_adj;          /**< Compass Z-Axis sensitivity adjustment value */
#endif
} mpu_status_t;

/**
 * @brief Device descriptor for the MPU- sensor
 */
typedef struct {
    i2c_t i2c_dev;                  /**< I2C device which is used */
    mpu_status_t conf;              /**< Device configuration */
    uint8_t hw_addr;                /**< Hardware address of the MPU- */
#ifdef MPU_WITH_COMPASS
    uint8_t comp_addr;              /**< Address of the MPU-s compass */
#endif
} mpu_dev_t;

/**
 * @brief Initialize the given MPU device
 *
 * @param[out] dev          Initialized device descriptor of MPU device
 * @param[in]  i2c          I2C bus the sensor is connected to
 * @param[in]  hw_addr      The device's address on the I2C bus
 * @param[in]  comp_addr    The compass address on the I2C bus
 *
 * @return                  0 on success
 * @return                  -1 if given I2C is not enabled in board config
 */
#ifdef MPU_WITH_COMPASS
int mpu_init(mpu_dev_t *dev, i2c_t i2c, mpu_hw_addr_t hw_addr,
             mpu_comp_addr_t comp_addr);
#else
int mpu_init(mpu_dev_t *dev, i2c_t i2c, mpu_hw_addr_t hw_addr);
#endif

/**
 * @brief Enable or disable accelerometer power
 *
 * @param[in] dev           Device descriptor of MPU device
 * @param[in] pwr_conf      Target power setting: PWR_ON or PWR_OFF
 *
 * @return                  0 on success
 * @return                  -1 if given I2C is not enabled in board config
 */
int mpu_set_accel_power(mpu_dev_t *dev, mpu_pwr_t pwr_conf);

/**
 * @brief Enable or disable gyroscope power
 *
 * @param[in] dev           Device descriptor of MPU device
 * @param[in] pwr_conf      Target power setting: PWR_ON or PWR_OFF
 *
 * @return                  0 on success
 * @return                  -1 if given I2C is not enabled in board config
 */
int mpu_set_gyro_power(mpu_dev_t *dev, mpu_pwr_t pwr_conf);

#ifdef MPU_WITH_COMPASS
/**
 * @brief Enable or disable compass power
 *
 * @param[in] dev           Device descriptor of MPU device
 * @param[in] pwr_conf      Target power setting: PWR_ON or PWR_OFF
 *
 * @return                  0 on success
 * @return                  -1 if given I2C is not enabled in board config
 */
int mpu_set_compass_power(mpu_dev_t *dev, mpu_pwr_t pwr_conf);
#endif /* MPU_WITH_COMPASS */
/**
 * @brief Read angular speed values from the given MPU device, returned in dps
 *
 * The raw gyroscope data is read from the sensor and normalized with respect to
 * the configured gyroscope full-scale range.
 *
 * @param[in]  dev          Device descriptor of MPU device to read from
 * @param[out] output       Result vector in dps per axis
 *
 * @return                  0 on success
 * @return                  -1 if device's I2C is not enabled in board config
 * @return                  -2 if gyro full-scale range is configured wrong
 */
int mpu_read_gyro(mpu_dev_t *dev, mpu_results_t *output);

/**
 * @brief Read acceleration values from the given MPU device, returned in mG
 *
 * The raw acceleration data is read from the sensor and normalized with respect to
 * the configured accelerometer full-scale range.
 *
 * @param[in]  dev          Device descriptor of MPU device to read from
 * @param[out] output       Result vector in mG per axis
 *
 * @return                  0 on success
 * @return                  -1 if device's I2C is not enabled in board config
 * @return                  -2 if accel full-scale range is configured wrong
 */
int mpu_read_accel(mpu_dev_t *dev, mpu_results_t *output);

#ifdef MPU_WITH_COMPASS
/**
 * @brief Read magnetic field values from the given MPU device, returned in mikroT
 *
 * The raw compass data is read from the sensor and normalized with respect to
 * the compass full-scale range (which can not be configured).
 *
 * @param[in]  dev          Device descriptor of MPU device to read from
 * @param[out] output       Result vector in mikroT per axis
 *
 * @return                  0 on success
 * @return                  -1 if device's I2C is not enabled in board config
 */
int mpu_read_compass(mpu_dev_t *dev, mpu_results_t *output);
#endif /* MPU_WITH_COMPASS */
/**
 * @brief Read temperature value from the given MPU device, returned in m°C
 *
 * @note
 * The measured temperature is slightly higher than the real room temperature.
 * Tests showed that the offset varied around 2-3 °C (but no warranties here).
 *
 * @param[in] dev           Device descriptor of MPU device to read from
 * @param[out] output       Temperature in m°C
 *
 * @return                  0 on success
 * @return                  -1 if device's I2C is not enabled in board config
 */
int mpu_read_temperature(mpu_dev_t *dev, int32_t *output);

/**
 * @brief Set the full-scale range for raw gyroscope data
 *
 * @param[in] dev           Device descriptor of MPU device
 * @param[in] fsr           Target full-scale range
 *
 * @return                  0 on success
 * @return                  -1 if device's I2C is not enabled in board config
 * @return                  -2 if given full-scale target value is not valid
 */
int mpu_set_gyro_fsr(mpu_dev_t *dev, mpu_gyro_ranges_t fsr);

/**
 * @brief Set the full-scale range for raw accelerometer data
 *
 * @param[in] dev           Device descriptor of MPU device
 * @param[in] fsr           Target full-scale range
 *
 * @return                  0 on success
 * @return                  -1 if device's I2C is not enabled in board config
 * @return                  -2 if given full-scale target value is not valid
 */
int mpu_set_accel_fsr(mpu_dev_t *dev, mpu_accel_ranges_t fsr);

/**
 * @brief Set the rate at which the gyroscope and accelerometer data is sampled
 *
 * Sample rate can be chosen between 4 Hz and 1kHz. The actual set value might
 * slightly differ. If necessary, check the actual set value in the device's
 * config member afterwards.
 *
 * @param[in] dev           Device descriptor of MPU device
 * @param[in] rate          Target sample rate in Hz
 *
 * @return                  0 on success
 * @return                  -1 if device's I2C is not enabled in board config
 * @return                  -2 if given target sample rate is not valid
 */
int mpu_set_sample_rate(mpu_dev_t *dev, uint16_t rate);

#ifdef MPU_WITH_COMPASS
/**
 * @brief Set the rate at which the compass data is sampled
 *
 * Sample rate can be chosen between 1 Hz and 100 Hz but has to be a fraction
 * of the configured accel/gyro sample rate. The actual set value might
 * slightly differ. If necessary, check the actual set value in the device's
 * config member afterwards.
 *
 * @param[in] dev           Device descriptor of MPU device
 * @param[in] rate          Target sample rate in Hz
 *
 * @return                  0 on success
 * @return                  -1 if device's I2C is not enabled in board config
 * @return                  -2 if given target sample rate is not valid
 */
int mpu_set_compass_sample_rate(mpu_dev_t *dev, uint8_t rate);
#endif /* MPU_WITH_COMPASS */

#ifdef __cplusplus
}
#endif

#endif /* MPU_COMMON_H_ */
/** @} */
