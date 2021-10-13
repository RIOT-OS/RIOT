/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_bmx055 BMX055 9-axis sensor
 * @ingroup     drivers_sensors
 * @ingroup     drivers_saul
 * @brief       Device driver for the Bosch BMX055 9-axis sensor
 *
 * This driver provides @ref drivers_saul capabilities.
 * @{
 *
 * @file
 * @brief       Device driver interface for the Bosch BMX055 9-axis sensor
 *
 * @note        The current state of the driver only implements basic polling.
 *
 * @author      Semjon Kerner <semjon.kerner@fu-berlin.de>
 */

#ifndef BMX055_H
#define BMX055_H

#include <stdint.h>

#include "periph/i2c.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup drivers_bmx055_config     BMX055 sensor driver compile configuration
 * @ingroup config_drivers_sensors
 * @{
 */
/**
 * @brief Magnetometer default address
 *
 * The address depends on the state of SDO1, SDO2 and CSB3 pins.
 * For more information refer to the section 'Inter-Integrated
 * Circuit (I²C)' in the datasheet.
 */
#ifndef CONFIG_BMX055_MAG_ADDR_DEFAULT
#define CONFIG_BMX055_MAG_ADDR_DEFAULT  (0x10U)
#endif

/**
 * @brief Accelerometer default address
 *
 * The address depends on the state of SDO1, SDO2 and CSB3 pins.
 * For more information refer to the section 'Inter-Integrated
 * Circuit (I²C)' in the datasheet.
 */
#ifndef CONFIG_BMX055_ACC_ADDR_DEFAULT
#define CONFIG_BMX055_ACC_ADDR_DEFAULT  (0x18U)
#endif

/**
 * @brief Gyroscope default address
 *
 * The address depends on the state of SDO1, SDO2 and CSB3 pins.
 * For more information refer to the section 'Inter-Integrated
 * Circuit (I²C)' in the datasheet.
 */
#ifndef CONFIG_BMX055_GYRO_ADDR_DEFAULT
#define CONFIG_BMX055_GYRO_ADDR_DEFAULT (0x68U)
#endif
/** @} */

/**
 * @brief   Status and error return codes
 */
enum {
    BMX055_OK       =  0,               /**< exit without error */
    BMX055_NOBUS    = -1,               /**< cannot connect to module on i2c bus */
    BMX055_NODEV    = -2,               /**< cannot read any data from module */
    BMX055_NOREAD   = -3,               /**< cannot read data from module */
    BMX055_NOWRITE  = -4,               /**< cannot write data to module */
    BMX055_NOTREADY = -5,               /**< no new data ready for reading */
};

/**
 * @brief   Datarate for Magnetometer
 */
typedef enum {
    BMX055_MAG_DRATE_DEFAULT    = 0x0,  /**< output data rate: 10 Hz */
    BMX055_MAG_DRATE_02HZ       = 0x1,  /**< output data rate:  2 Hz */
    BMX055_MAG_DRATE_06HZ       = 0x2,  /**< output data rate:  6 Hz */
    BMX055_MAG_DRATE_08HZ       = 0x3,  /**< output data rate:  8 Hz*/
    BMX055_MAG_DRATE_15HZ       = 0x4,  /**< output data rate: 15 Hz */
    BMX055_MAG_DRATE_20HZ       = 0x5,  /**< output data rate: 20 Hz */
    BMX055_MAG_DRATE_25HZ       = 0x6,  /**< output data rate: 25 Hz */
    BMX055_MAG_DRATE_30HZ       = 0x7,  /**< output data rate: 30 Hz */
} bmx055_mag_rate_t;

/**
 * @brief   Range for Accelerometer
 */
typedef enum {
    BMX055_ACC_RANGE_2G     = 0x0,      /**< range: 2g */
    BMX055_ACC_RANGE_4G     = 0x1,      /**< range: 4g */
    BMX055_ACC_RANGE_8G     = 0x2,      /**< range: 8g */
    BMX055_ACC_RANGE_16G    = 0x3,      /**< range: 16g */
} bmx055_acc_range_t;

/**
 * @brief   Measurement scale for the gyro
 */
typedef enum {
    BMX055_GYRO_SCALE_2000DPS = 0x0,    /**< scale: 2000 degree per second */
    BMX055_GYRO_SCALE_1000DPS = 0x1,    /**< scale: 1000 degree per second */
    BMX055_GYRO_SCALE_0500DPS = 0x2,    /**< scale: 500 degree per second */
    BMX055_GYRO_SCALE_0250DPS = 0x3,    /**< scale: 250 degree per second */
    BMX055_GYRO_SCALE_0125DPS = 0x4,    /**< scale: 125 degree per second */
} bmx055_gyro_scale_t;

/**
 * @brief   Data structure holding the device parameters needed for initialization
 */
typedef struct {
    i2c_t i2c;                          /**< I2C bus the device is connected to */
    uint8_t addr_mag;                   /**< the magnetometer address on that bus */
    uint8_t addr_acc;                   /**< the accelerometer address on that bus */
    uint8_t addr_gyro;                  /**< the gyroscope address on that bus */
    gpio_t int1_pin;                    /**< GPIO pin connected to the INT1 line */
    gpio_t int2_pin;                    /**< GPIO pin connected to the INT2 line */
    uint8_t mag_rate;                   /**< datarate of magnetometer */
    uint8_t acc_range;                  /**< range of accelerometer */
    uint8_t gyro_scale;                 /**< range of gyroscope*/
} bmx055_params_t;

/**
 * @brief   Device descriptor for BMX055 sensors
 */
typedef struct {
    bmx055_params_t p;  /**< Device initialization parameters*/
} bmx055_t;

/**
 * @brief   Initialize modules magnetometer, accelerometer, gyroscope
 *
 * @param[out] dev      device descriptor of sensor to initialize
 * @param[in]  params   default parameter values
 *
 * @return              BMX055_OK on success
 * @return              BMX055_NOBUS if i2C connection can not be establish
 * @return              BMX055_NODEV if the register of a  module can not be read
 * @return              BMX055_NOWRITE if a register can not be written
 */
int bmx055_init(bmx055_t *dev, const bmx055_params_t *params);

/**
 * @brief   Read magnetic field value in Gauss per second from magnetometer
 *
 * @param[in]  dev      device descriptor of magnetometer
 * @param[out] data     result vector in Gs per axis
 *
 * @return              BMX055_OK on success
 * @return              BMX055_NOREAD if reading mag data is not possible
 * @return              BMX055_NOTRDY if no new data is available
 */
int bmx055_mag_read(const bmx055_t *dev, int16_t *data);

 /**
 * @brief   Read acceleration value in g from accelerometer
 *
 * @param[in]  dev      device descriptor of accelerometer
 * @param[out] data     result vector in g per axis
 *
 * @return              BMX055_OK on success
 * @return              BMX055_NOREAD if reading acc data is not possible
 * @return              BMX055_NOTRDY if no new data is available
 */
int bmx055_acc_read(const bmx055_t *dev, int16_t *data);

/**
 * @brief   Read angular speed value in degree per second from gyroscope
 *
 * @note    The data of steady axis deviate from the expected values while
 * moving the sensor in one of the other axis. Hence reading data for all axis
 * at once may not always give the expected results.
 *
 * @param[in]  dev      device descriptor of gyroscope
 * @param[out] data     result vector in dps per axis
 *
 * @return              BMX055_OK on success
 * @return              BMX055_NOREAD if reading gyro data is not possible
 */
int bmx055_gyro_read(const bmx055_t *dev, int16_t *data);

#ifdef __cplusplus
}
#endif

#endif /* BMX055_H */
/** @} */
