/*
 * Copyright (C) 2019 Wouter Horlings
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_bno055 BNO055 9-axis sensor
 * @ingroup     drivers_sensors
 * @ingroup     drivers_saul
 * @brief       Device driver for the Bosch BNO055 9-axis sensor
 *
 * This driver provides @ref drivers_saul capabilities.
 * @{
 *
 * @file
 * @brief       Device driver interface for the Bosch BNO055 9-axis sensor
 * This sensor is an 9-DOF IMU with integrated sensorfusion. This makes it
 * possible to download rotation matrices and orientations directly from the
 * registers.
 * This is the reason for the scaling float functions. To use the sensor fusion
 * data directly in your code it has to be scaled.
 * A quaternion representation is worthless if the vector length != 1
 *
 * Furthermore, it has the following fusion outputs:
 *   - Quarternion vector.
 *   - Euler rotation vector.
 *   - Linear Acceration Vector
 *   - Gravity Vector
 *
 * @note        The current state of the driver only implements basic polling.
 *
 * @author      Wouter Horlings <wouter@horlin.gs>
 */

#ifndef BNO055_H
#define BNO055_H

#include <stdint.h>
#include <errno.h>

#include "periph/i2c.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief contains 8 unsigned bytes and 4 signed words.
 */
typedef union {
    uint8_t u8bit[8];   /**< Array of 8 unsigned bytes */
    int16_t i16_bit[4]; /**< Array of 4 signed words */
} bno055_4bit16_t;

/**
 * @brief contains 6 unsigned bytes and 3 signed words.
 */
typedef union {
    uint8_t u8bit[6];   /**< Array of 6 unsigned bytes */
    int16_t i16_bit[3]; /**< Array of 3 signed words */
} bno055_3bit16_t;

/**
 * @brief Representing the four components of a quaternion
 */
typedef struct {
    float w;    /**< Real component of quaternion */
    float x;    /**< First imaginary component of quaternion */
    float y;    /**< Second imaginary component of quaternion */
    float z;    /**< Third imaginary component of quaternion */
} quat_t;

/**
 * @brief Representing an Euler angle rotation
 */
typedef struct {
    float heading;  /**< Heading component of euler angles */
    float roll;     /**< Roll component of euler angles */
    float pitch;    /**< Pitch component of euler angles */
} eul_t;

/**
 * @brief   Euclidian space representation structure
 * To represent a position in three dimensional space.
 */
typedef struct {
    float x;    /**< x component */
    float y;    /**< y component */
    float z;    /**< z component */
} euclid_t;

/**
 * @name   The sensor default I2C address
 * @{
 */
#define BNO055_ADDR_DEFAULT  (0x29U)
/** @} */

/**
 * @brief   Status and error return codes
 */
enum {
    BNO055_OK       = 0,      /**< exit without error */
    BNO055_NOBUS    = ENXIO,  /**< cannot connect to module on i2c bus */
    BNO055_NODEV    = ENODEV, /**< cannot read any data from module */
    BNO055_NORW     = EIO,    /**< cannot read data from module */
    BNO055_NOTREADY = EBUSY,  /**< no new data ready for reading */
};

/**
 * @brief   Mode for sensor operation
 */
typedef enum {
    BNO055_OPR_MODE_CONFIG          = 0x0,  /**< operation mode: config - no sensors available */
    BNO055_OPR_MODE_ACCONLY         = 0x1,  /**< operation mode: no-fusion - accelerometer only */
    BNO055_OPR_MODE_MAGONLY         = 0x2,  /**< operation mode: no-fusion - magnetometer only */
    BNO055_OPR_MODE_GYROONLY        = 0x3,  /**< operation mode: no-fusion - gyroscope only */
    BNO055_OPR_MODE_ACCMAG          = 0x4,  /**< operation mode: no-fusion - accelerometer and magnetometer only */
    BNO055_OPR_MODE_ACCGYRO         = 0x5,  /**< operation mode: no-fusion - accelerometer and gyroscope only */
    BNO055_OPR_MODE_MAGGYRO         = 0x6,  /**< operation mode: no-fusion - magnetometer and gyroscope only */
    BNO055_OPR_MODE_AMG             = 0x7,  /**< operation mode: no-fusion - all sensors enabled */
    BNO055_OPR_MODE_IMU             = 0x8,  /**< operation mode: fusion - IMU mode with relative orientation */
    BNO055_OPR_MODE_COMPASS         = 0x9,  /**< operation mode: fusion - compass mode with absolute orientation (calibration required) */
    BNO055_OPR_MODE_M4G             = 0xA,  /**< operation mode: fusion - magnet for gyroscope, similar to IMU but absolute orientation */
    BNO055_OPR_MODE_NDOF_FMC_OFF    = 0xB,  /**< operation mode: fusion - full sensor fusion without Fast Magnetometer Calibration */
    BNO055_OPR_MODE_NDOF            = 0xC,  /**< operation mode: fusion - full sensor fusion with Fast Magnetometer Calibration */
} bno055_opr_mode_t;

/**
 * @brief   Power mode for sensor
 */
typedef enum {
    BNO055_PWR_MODE_NORMAL  = 0x0, /**< All sensors required are always switched ON */
    BNO055_PWR_MODE_LOW     = 0x1, /**< If no motion is detected for a specified duration, all sensors except
                                     *  the accelerometer is switched off. Once motion is detected, the system
                                     *  is woken and normal mode is entered. */
    BNO055_PWR_MODE_SUSPEND = 0x2, /**< Sensors and MCU are put into sleep mode. Exit from mode by switching
                                     *  to Normal or low Power mode. */
} bno055_pwr_mode_t;

/**
 * @brief   Datarate for Magnetometer
 */
typedef enum {
    BNO055_MAG_DRATE_DEFAULT    = 0x0,  /**< output data rate: 10 Hz */
    BNO055_MAG_DRATE_02HZ       = 0x1,  /**< output data rate:  2 Hz */
    BNO055_MAG_DRATE_06HZ       = 0x2,  /**< output data rate:  6 Hz */
    BNO055_MAG_DRATE_08HZ       = 0x3,  /**< output data rate:  8 Hz */
    BNO055_MAG_DRATE_15HZ       = 0x4,  /**< output data rate: 15 Hz */
    BNO055_MAG_DRATE_20HZ       = 0x5,  /**< output data rate: 20 Hz */
    BNO055_MAG_DRATE_25HZ       = 0x6,  /**< output data rate: 25 Hz */
    BNO055_MAG_DRATE_30HZ       = 0x7,  /**< output data rate: 30 Hz */
} bno055_mag_rate_t;

/**
 * @brief   Range for Accelerometer
 */
typedef enum {
    BNO055_ACC_RANGE_2G     = 0x0,      /**< range: 2g */
    BNO055_ACC_RANGE_4G     = 0x1,      /**< range: 4g */
    BNO055_ACC_RANGE_8G     = 0x2,      /**< range: 8g */
    BNO055_ACC_RANGE_16G    = 0x3,      /**< range: 16g */
} bno055_acc_range_t;

/**
 * @brief   Measurement scale for the gyro
 */
typedef enum {
    BNO055_GYRO_SCALE_2000DPS   = 0x0,  /**< scale: 2000 degree per second */
    BNO055_GYRO_SCALE_1000DPS   = 0x1,  /**< scale: 1000 degree per second */
    BNO055_GYRO_SCALE_0500DPS   = 0x2,  /**< scale: 500 degree per second */
    BNO055_GYRO_SCALE_0250DPS   = 0x3,  /**< scale: 250 degree per second */
    BNO055_GYRO_SCALE_0125DPS   = 0x4,  /**< scale: 125 degree per second */
} bno055_gyro_scale_t;

/**
 * @brief   Data structure holding the device parameters needed for initialization
 */
typedef struct {
    i2c_t i2c;          /**< I2C bus the device is connected to */
    uint8_t addr;       /**< The I2C device address */
    uint8_t opr;        /**< Operation mode of sensor */
    uint8_t pwr;        /**< Power mode of sensor */
    uint8_t unit_sel;   /**< Byte with unit settings. */
    uint32_t gpio1;     /**< Set gpio pin 1. */
    uint32_t gpio2;     /**< Set gpio pin 2. */
    uint8_t mag_rate;   /**< Magnetometer rate */
    uint8_t acc_range;  /**< Accelerometer Range */
    uint8_t acc_unit;   /**< Accelerometer Unit */
    uint8_t gyro_scale; /**< Gyroscope scale */
} bno055_params_t;

/**
 * @brief   Device descriptor for BNO055 sensors
 */
typedef struct {
    const bno055_params_t *p;  /**< Device initialization parameters*/
} bno055_t;

/**
 * @brief   Initialize modules magnetometer, accelerometer, gyroscope
 *
 * @param[out] dev      device descriptor of sensor to initialize
 * @param[in]  params   default parameter values
 *
 * @retval              BNO055_OK on success
 * @retval              BNO055_NOBUS if i2C connection can not be establish
 * @retval              BNO055_NODEV if the register of a  module can not be read
 * @retval              BNO055_NOWRITE if a register can not be written
 */
int bno055_init(bno055_t *dev, const bno055_params_t *params);

/**
 * @brief   Read quaternion orientation data.
 *
 * | Unit | Representation | Scaling           |
 * |------|----------------|-------------------|
 * | -   | 1.0 = 2^14 LSB  | BNO055_DIV_SCALE_QUAT_UN |
 *
 * @param[in]  dev      device descriptor of IMU
 * @param[out] data     4 word struct 'bno055_4bit16_t' with quaternion vector
 *
 * @retval              BNO055_OK on success
 * @retval              BNO055_NOREAD if reading mag data is not possible
 * @retval              BNO055_NOTRDY if no new data is available
 */
int bno055_quat_read(const bno055_t *dev, bno055_4bit16_t *data);

/**
 * @brief   Read orientation in Euler representation.
 *
 * | Unit    | Representation      | \#define BNO055_PARAM_EUL_UNIT  | Scaling                  |         |
 * |---------|---------------------|--------------------------------|--------------------------|---------|
 * | degrees | 1 degree = 16 LSB   | BNO055_BIT_EUL_DEG             | BNO055_DIV_SCALE_EUL_DEG |         |
 * | radians | 1 radians = 900 LSB | BNO055_BIT_EUL_RAD             | BNO055_DIV_SCALE_EUL_RAD | default |
 *
 * @param[in]  dev      device descriptor of IMU
 * @param[out] data     3 word struct 'bno055_3bit16_t'
 *
 * @retval              BNO055_OK on success
 * @retval              BNO055_NOREAD if reading mag data is not possible
 * @retval              BNO055_NOTRDY if no new data is available
 */
int bno055_eul_read(const bno055_t *dev, bno055_3bit16_t *data);

/**
 * @brief   Read linear acceleration as euclidian vector.
 *
 * | Unit | Representation   | \#define BNO055_PARAM_ACC_UNIT | Scaling                  |          |
 * |------|------------------|-------------------------------|--------------------------|----------|
 * | m/s2 | 1 m/s2 = 100 LSB | BNO055_BIT_ACC_MSS            | BNO055_DIV_SCALE_ACC_MSS | (default)|
 * | mg   | 1 mg = 1 LSB     | BNO055_BIT_ACC_G              | BNO055_DIV_SCALE_ACC_G   |          |
 *
 * @param[in]  dev      device descriptor of IMU
 * @param[out] data     3 word struct 'bno055_3bit16_t'
 *
 * @retval              BNO055_OK on success
 * @retval              BNO055_NOREAD if reading mag data is not possible
 * @retval              BNO055_NOTRDY if no new data is available
 */
int bno055_lia_read(const bno055_t *dev, bno055_3bit16_t *data);

/**
 * @brief   Read orientation as gravity vector.
 *
 * | Unit | Representation   | \#define BNO055_PARAM_ACC_UNIT | Scaling                  |          |
 * |------|------------------|-------------------------------|--------------------------|----------|
 * | m/s2 | 1 m/s2 = 100 LSB | BNO055_BIT_ACC_MSS            | BNO055_DIV_SCALE_ACC_MSS | (default)|
 * | mg   | 1 mg = 1 LSB     | BNO055_BIT_ACC_G              | BNO055_DIV_SCALE_ACC_G   |          |
 *
 * @param[in]  dev      device descriptor of IMU
 * @param[out] data     3 word struct 'bno055_3bit16_t'
 *
 * @retval              BNO055_OK on success
 * @retval              BNO055_NOREAD if reading mag data is not possible
 * @retval              BNO055_NOTRDY if no new data is available
 */
int bno055_grv_read(const bno055_t *dev, bno055_3bit16_t *data);

/**
 * @brief   Read raw acceleration data.
 *
 * | Unit | Representation   | \#define BNO055_PARAM_ACC_UNIT | Scaling                  |          |
 * |------|------------------|-------------------------------|--------------------------|----------|
 * | m/s2 | 1 m/s2 = 100 LSB | BNO055_BIT_ACC_MSS            | BNO055_DIV_SCALE_ACC_MSS | (default)|
 * | mg   | 1 mg = 1 LSB     | BNO055_BIT_ACC_G              | BNO055_DIV_SCALE_ACC_G   |          |
 *
 * @param[in]  dev      device descriptor of IMU
 * @param[out] data     3 word struct 'bno055_3bit16_t'
 *
 * @retval              BNO055_OK on success
 * @retval              BNO055_NOREAD if reading mag data is not possible
 * @retval              BNO055_NOTRDY if no new data is available
 */
int bno055_acc_read(const bno055_t *dev, bno055_3bit16_t *data);

/**
 * @brief   Read raw magnetic field data.
 *
 * | Unit | Representation | Scaling           |
 * |------|----------------|-------------------|
 * | μT   | 1 μT = 16 LSB  | BNO055_DIV_SCALE_MAG |
 *
 * @param[in]  dev      device descriptor of IMU
 * @param[out] data     3 word struct 'bno055_3bit16_t'
 *
 * @retval              BNO055_OK on success
 * @retval              BNO055_NOREAD if reading mag data is not possible
 * @retval              BNO055_NOTRDY if no new data is available
 */
int bno055_mag_read(const bno055_t *dev, bno055_3bit16_t *data);

/**
 * @brief   Read raw gyroscope data.
 *
 * | Unit     | Representation      | \#define BNO055_PARAM_GYR_UNIT  | Scaling                  |         |
 * |----------|---------------------|--------------------------------|--------------------------|---------|
 * | degree/s | 1 degree/s = 16 LSB | BNO055_BIT_GYR_DPS             | BNO055_DIV_SCALE_GYR_DPS |         |
 * | rad/s    | 1 rad/s = 900 LSB   | BNO055_BIT_GYR_RPS             | BNO055_DIV_SCALE_GYR_RPS | default |
 * @param[in]  dev      device descriptor of IMU
 * @param[out] data     3 word struct 'bno055_3bit16_t'
 *
 * @retval              BNO055_OK on success
 * @retval              BNO055_NOREAD if reading mag data is not possible
 * @retval              BNO055_NOTRDY if no new data is available
 */
int bno055_gyr_read(const bno055_t *dev, bno055_3bit16_t *data);

/**
 * @brief   change the current I2C register page
 *
 * @param[in]  dev      device descriptor of IMU
 * @param[in]  page     desired page number (0 or 1)
 *
 * @retval              BNO055_OK on success
 * @retval              BNO055_NOREAD if reading mag data is not possible
 * @retval              BNO055_NOTRDY if no new data is available
 */
int bno055_set_page(const bno055_t *dev, uint8_t page);

/**
 * @brief   Set operation mode of the device
 *
 * @param[in]  dev      device descriptor of IMU
 * @param[in]  mode     bno055_opr_mode_t with the desired mode
 *
 * @retval              BNO055_OK on success
 * @retval              BNO055_NOREAD if reading mag data is not possible
 * @retval              BNO055_NOTRDY if no new data is available
 */
int bno055_set_mode(const bno055_t *dev, bno055_opr_mode_t mode);

/**
 * @brief   Read and scale the quaterion rotation vector.
 *
 * @param[in]  dev      device descriptor of IMU
 * @param[out] data     4 word struct 'bno055_4bit16_t'
 *
 * @retval              BNO055_OK on success
 * @retval              BNO055_NOREAD if reading mag data is not possible
 * @retval              BNO055_NOTRDY if no new data is available
 */
int bno055_quat_float(const bno055_t *dev, quat_t *data);

/**
 * @brief   Read and scale the euler rotation vector.
 *
 * @param[in]  dev      device descriptor of IMU
 * @param[out] data     3 word struct 'bno055_3bit16_t'
 *
 * @retval              BNO055_OK on success
 * @retval              BNO055_NOREAD if reading mag data is not possible
 * @retval              BNO055_NOTRDY if no new data is available
 */
int bno055_eul_float(const bno055_t *dev, eul_t *data);

/**
 * @brief   Read and scale orientation as gravity vector.
 * This is a x,y,z vector that points in the direction of gravity.
 * Unit for acceleration is set to m/s2 by default. But can be changes to 'g'.
 *
 * @param[in]  dev      device descriptor of IMU
 * @param[out] data     3 word struct 'bno055_3bit16_t'
 *
 * @retval              BNO055_OK on success
 * @retval              BNO055_NOREAD if reading mag data is not possible
 * @retval              BNO055_NOTRDY if no new data is available
 */
int bno055_grv_float(const bno055_t *dev, euclid_t *data);

/**
 * @brief   Read and scale Linear acceleration vector
 * This is a x,y,z vector that represents the acceleration due to movement.
 * Unit for acceleration is set to m/s2 by default. But can be changes to 'g'.
 *
 * @param[in]  dev      device descriptor of IMU
 * @param[out] data     3 word struct 'bno055_3bit16_t'
 *
 * @retval              BNO055_OK on success
 * @retval              BNO055_NOREAD if reading mag data is not possible
 * @retval              BNO055_NOTRDY if no new data is available
 */
int bno055_lia_float(const bno055_t *dev, euclid_t *data);

/**
 * @brief   Read and scale the accelerometer vector.
 * This x,y,z vector is the raw acceleration value.
 * Also the sum of the gravitation vector and the linear acceleration vector.
 * Unit for acceleration is set to m/s2 by default. But can be changes to 'g'.
 *
 * @param[in]  dev      device descriptor of IMU
 * @param[out] data     3 word struct 'bno055_3bit16_t'
 *
 * @retval              BNO055_OK on success
 * @retval              BNO055_NOREAD if reading mag data is not possible
 * @retval              BNO055_NOTRDY if no new data is available
 */
int bno055_acc_float(const bno055_t *dev, euclid_t *data);

/**
 * @brief   Read and scale the magnetic field strength vector.
 * Default value is in microTesla.
 *
 * @param[in]  dev      device descriptor of IMU
 * @param[out] data     3 word struct 'bno055_3bit16_t'
 *
 * @retval              BNO055_OK on success
 * @retval              BNO055_NOREAD if reading mag data is not possible
 * @retval              BNO055_NOTRDY if no new data is available
 */
int bno055_mag_float(const bno055_t *dev, euclid_t *data);

/**
 * @brief   Read and scale gyroscope vector.
 * Rotation is given in radians per second.
 *
 * @param[in]  dev      device descriptor of IMU
 * @param[out] data     3 word struct 'bno055_3bit16_t'
 *
 * @retval              BNO055_OK on success
 * @retval              BNO055_NOREAD if reading mag data is not possible
 * @retval              BNO055_NOTRDY if no new data is available
 */
int bno055_gyr_float(const bno055_t *dev, euclid_t *data);

#ifdef __cplusplus
}
#endif

#endif /* BNO055_H */
/** @} */
