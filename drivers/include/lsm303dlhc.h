/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_lsm303dlhc LSM303DLHC 3D accelerometer/magnetometer
 * @ingroup     drivers_sensors
 * @brief       Device driver for the LSM303DLHC 3D accelerometer/magnetometer
 * @{
 *
 * @file
 * @brief       Device driver interface for the LSM303DLHC 3D accelerometer/magnetometer
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 */

#ifndef LSM303DLHC_H
#define LSM303DLHC_H

#include <stdint.h>
#include "periph/i2c.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The sensors default I2C address
 */
#define LSM303DLHC_ACC_DEFAULT_ADDRESS        (0x19)
#define LSM303DLHC_MAG_DEFAULT_ADDRESS        (0x1e)

/**
 * @brief Possible accelerometer sample rates
 */
typedef enum {
    LSM303DLHC_ACC_SAMPLE_RATE_1HZ              = 0x10, /**< 1Hz sample rate       */
    LSM303DLHC_ACC_SAMPLE_RATE_10HZ             = 0x20, /**< 10Hz sample rate      */
    LSM303DLHC_ACC_SAMPLE_RATE_25HZ             = 0x30, /**< 25Hz sample rate      */
    LSM303DLHC_ACC_SAMPLE_RATE_50HZ             = 0x40, /**< 50Hz sample rate      */
    LSM303DLHC_ACC_SAMPLE_RATE_100HZ            = 0x50, /**< 100Hz sample rate     */
    LSM303DLHC_ACC_SAMPLE_RATE_200HZ            = 0x60, /**< 200Hz sample rate     */
    LSM303DLHC_ACC_SAMPLE_RATE_400HZ            = 0x70, /**< 400Hz sample rate     */
    LSM303DLHC_ACC_SAMPLE_RATE_1620HZ           = 0x80, /**< 1620Hz sample rate    */
    LSM303DLHC_ACC_SAMPLE_RATE_N1344HZ_L5376HZ  = 0x90  /**< 1344Hz normal mode,
                                                         *   5376Hz low power mode */
} lsm303dlhc_acc_sample_rate_t;

/**
 * @brief Possible accelerometer scales
 */
typedef enum {
    LSM303DLHC_ACC_SCALE_2G     = 0x00, /**< +- 2g range */
    LSM303DLHC_ACC_SCALE_4G     = 0x10, /**< +- 4g range */
    LSM303DLHC_ACC_SCALE_8G     = 0x20, /**< +- 8g range */
    LSM303DLHC_ACC_SCALE_16G    = 0x30, /**< +-16g range */
} lsm303dlhc_acc_scale_t;

/**
 * @brief Possible magnetometer sample rates
 */
typedef enum {
    LSM303DLHC_MAG_SAMPLE_RATE_0_75HZ   = 0x00, /**< 0.75Hz sample rate */
    LSM303DLHC_MAG_SAMPLE_RATE_1_5HZ    = 0x04, /**< 1.5 Hz sample rate */
    LSM303DLHC_MAG_SAMPLE_RATE_3HZ      = 0x08, /**< 3   Hz sample rate */
    LSM303DLHC_MAG_SAMPLE_RATE_7_5HZ    = 0x0c, /**< 7.5 Hz sample rate */
    LSM303DLHC_MAG_SAMPLE_RATE_15HZ     = 0x10, /**< 15  Hz sample rate */
    LSM303DLHC_MAG_SAMPLE_RATE_30HZ     = 0x14, /**< 30  Hz sample rate */
    LSM303DLHC_MAG_SAMPLE_RATE_75HZ     = 0x18, /**< 75  Hz sample rate */
    LSM303DLHC_MAG_SAMPLE_RATE_220HZ    = 0x1c, /**< 220 Hz sample rate */
} lsm303dlhc_mag_sample_rate_t;

/**
 * @brief Possible magnetometer gain values
 */
 typedef enum {
    LSM303DLHC_MAG_GAIN_1100_980_GAUSS = 0x20, /**< 1100Gauss XYZ 980Gauss Z */
    LSM303DLHC_MAG_GAIN_855_760_GAUSS  = 0x40, /**<  855Gauss XYZ 760Gauss Z */
    LSM303DLHC_MAG_GAIN_670_600_GAUSS  = 0x60, /**<  670Gauss XYZ 600Gauss Z */
    LSM303DLHC_MAG_GAIN_450_400_GAUSS  = 0x80, /**<  450Gauss XYZ 400Gauss Z */
    LSM303DLHC_MAG_GAIN_400_355_GAUSS  = 0xa0, /**<  400Gauss XYZ 355Gauss Z */
    LSM303DLHC_MAG_GAIN_330_295_GAUSS  = 0xc0, /**<  330Gauss XYZ 295Gauss Z */
    LSM303DLHC_MAG_GAIN_230_205_GAUSS  = 0xe0, /**<  230Gauss XYZ 205Gauss Z */
 } lsm303dlhc_mag_gain_t;

/**
 * @brief 3d data container
 */
typedef struct {
    int16_t x_axis;
    int16_t y_axis;
    int16_t z_axis;
} lsm303dlhc_3d_data_t;

/**
 * @brief Device descriptor for LSM303DLHC sensors
 */
typedef struct {
    i2c_t i2c;              /**< I2C device                  */
    uint8_t acc_address;    /**< accelerometer's I2C address */
    uint8_t mag_address;    /**< magnetometer's I2C address  */
    gpio_t  acc_pin;        /**< accelerometer's data ready pin */
    gpio_t  mag_pin;        /**< magnetometer's data ready pin  */
} lsm303dlhc_t;

/**
 * @brief Data structure holding all the information needed for initialization
 */
typedef struct {
    i2c_t i2c;                              /**< I2C bus used */
    uint8_t acc_addr;                       /**< accelerometer I2C address */
    gpio_t acc_pin;                         /**< accelerometer EXTI pin */
    lsm303dlhc_acc_sample_rate_t acc_rate;  /**< accelerometer sample rate */
    lsm303dlhc_acc_scale_t acc_scale;       /**< accelerometer scale factor */
    uint8_t mag_addr;                       /**< magnetometer I2C address */
    gpio_t mag_pin;                         /**< magnetometer EXTI pin */
    lsm303dlhc_mag_sample_rate_t mag_rate;  /**< magnetometer sample rate */
    lsm303dlhc_mag_gain_t mag_gain;         /**< magnetometer gain */
} lsm303dlhc_params_t;

/**
 * @brief Initialize a new LSM303DLHC device
 *
 * @param[in] dev               device descriptor of an LSM303DLHC device
 * @param[in] i2c               I2C device the sensor is connected to
 * @param[in] acc_pin           GPIO pin connected to accelerometer
 * @param[in] mag_pin           GPIO pin connected to magnetometer
 * @param[in] acc_address       I2C address of the accelerometer
 * @param[in] acc_sample_rate   accelerometer sample rate
 * @param[in] acc_scale         accelerometer scale (from +- 2g to +-16g)
 * @param[in] mag_address       I2C address of the magnetometer
 * @param[in] mag_sample_rate   magnetometer sample rate
 * @param[in] mag_gain          magnetometer gain
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int lsm303dlhc_init(lsm303dlhc_t *dev, i2c_t i2c, gpio_t acc_pin, gpio_t mag_pin,
                    uint8_t acc_address,
                    lsm303dlhc_acc_sample_rate_t acc_sample_rate,
                    lsm303dlhc_acc_scale_t acc_scale,
                    uint8_t mag_address,
                    lsm303dlhc_mag_sample_rate_t mag_sample_rate,
                    lsm303dlhc_mag_gain_t mag_gain);

/**
 * @brief Read a accelerometer value from the sensor.
 *
 * @details This function provides raw acceleration data. To get the
 *          corresponding values in g please refer to the following
 *          table:
 *                measurement range | factor
 *              --------------------+---------
 *                          +- 2g   |  1*10^-3
 *                          +- 4g   |  2*10^-3
 *                          +- 8g   |  4*10^-3
 *                          +-16g   | 12*10^-3
 *
 * @param[in]  dev      device descriptor of an LSM303DLHC device
 * @param[out] data     the measured accelerometer data
 *
 * @return              0 on success
 * @return              -1 on error
 */
int lsm303dlhc_read_acc(lsm303dlhc_t *dev, lsm303dlhc_3d_data_t *data);

/**
 * @brief Read a magnetometer value from the sensor.
 *
 * @details This function returns raw magnetic data. To get the
 *          corresponding values in gauss please refer to the following
 *          table:
 *                measurement range |  factor
 *              --------------------+---------
 *                     +- 1.3 gauss |  1/1100
 *                     +- 1.9 gauss |  1/855
 *                     +- 2.5 gauss |  1/670
 *                     +- 4.0 gauss |  1/450
 *                     +- 4.7 gauss |  1/400
 *                     +- 5.6 gauss |  1/330
 *                     +- 8.1 gauss |  1/230
 *
 * @param[in]  dev      device descriptor of an LSM303DLHC device
 * @param[out] data     the measured magnetometer data
 *
 * @return              0 on success
 * @return              -1 on error
 */
int lsm303dlhc_read_mag(lsm303dlhc_t *dev, lsm303dlhc_3d_data_t *data);

/**
 * @brief Read a temperature value from the sensor.
 *
 * @param[in]  dev      device descriptor of an LSM303DLHC device
 * @param[out] value    the measured temperature
 *
 * @return              0 on success
 * @return              -1 on error
 */
int lsm303dlhc_read_temp(lsm303dlhc_t *dev, int16_t *value);

/**
 * @brief Enable the given sensor
 *
 * @param[in] dev       device descriptor of an LSM303DLHC device
 *
 * @return              0 on success
 * @return              -1 on error
 */
int lsm303dlhc_enable(lsm303dlhc_t *dev);

/**
 * @brief Disable the given sensor
 *
 * @param[in] dev       device descriptor of an LSM303DLHC device
 *
 * @return              0 on success
 * @return              -1 on error
 */
int lsm303dlhc_disable(lsm303dlhc_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* LSM303DLHC_H */
 /** @} */
