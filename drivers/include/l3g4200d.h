/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_l3g4200d L3G4200D gyroscope
 * @ingroup     drivers_sensors
 * @ingroup     drivers_saul
 * @brief       Device driver for the L3G4200D gyroscope
 *
 * This driver provides @ref drivers_saul capabilities.
 *
 * @note The current state of the driver only implements a very basic polling mode.
 *
 * @{
 *
 * @file
 * @brief       Device driver interface for the L3G4200D gyroscope
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef L3G4200D_H
#define L3G4200D_H

#include <stdint.h>

#include "periph/i2c.h"
#include "periph/gpio.h"

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @defgroup drivers_l3g4200d_config     L3G4200D gyroscope driver compile configuration
 * @ingroup config_drivers_sensors
 * @{
 */
/**
 * @brief Default address
 *
 * The address depends on the status of SDO Pin. Default
 * address corresponds to SDO connected to GND. For more
 * information refer to the section 'I2C Operation' in
 * the datasheet.
 */
#ifndef CONFIG_L3G4200D_DEFAULT_ADDRESS
#define CONFIG_L3G4200D_DEFAULT_ADDRESS        0x68
#endif
/** @} */

/**
 * @brief   Result vector for gyro measurement
 */
typedef struct {
    int16_t acc_x;          /**< roll rate in dgs (degree per second) */
    int16_t acc_y;          /**< pitch rate in dgs */
    int16_t acc_z;          /**< yaw rate in dgs */
} l3g4200d_data_t;

/**
 * @brief   Measurement scale for the gyro
 */
typedef enum {
    L3G4200D_SCALE_250DPS  = 0x0,       /**< scale: 250 degree per second */
    L3G4200D_SCALE_500DPS  = 0x1,       /**< scale: 500 degree per second */
    L3G4200D_SCALE_2000DPS = 0x2        /**< scale: 2000 degree per second */
} l3g4200d_scale_t;

/**
 * @brief   Sampling frequency and bandwidth settings for the gyro
 */
typedef enum {
    L3G4200D_MODE_100_12  = 0x0,        /**< data rate: 100Hz, cut-off: 12.5Hz */
    L3G4200D_MODE_100_25  = 0x1,        /**< data rate: 100Hz, cut-off: 25Hz */
    L3G4200D_MODE_200_12  = 0x4,        /**< data rate: 200Hz, cut-off: 12.5Hz */
    L3G4200D_MODE_200_25  = 0x5,        /**< data rate: 200Hz, cut-off: 25Hz */
    L3G4200D_MODE_200_50  = 0x6,        /**< data rate: 200Hz, cut-off: 50Hz */
    L3G4200D_MODE_200_70  = 0x7,        /**< data rate: 200Hz, cut-off: 70Hz */
    L3G4200D_MODE_400_20  = 0x8,        /**< data rate: 400Hz, cut-off: 20Hz */
    L3G4200D_MODE_400_25  = 0x9,        /**< data rate: 400Hz, cut-off: 25Hz */
    L3G4200D_MODE_400_50  = 0xa,        /**< data rate: 400Hz, cut-off: 50Hz */
    L3G4200D_MODE_400_110 = 0xb,        /**< data rate: 400Hz, cut-off: 110Hz */
    L3G4200D_MODE_800_30  = 0xc,        /**< data rate: 800Hz, cut-off: 30Hz */
    L3G4200D_MODE_800_35  = 0xd,        /**< data rate: 800Hz, cut-off: 35Hz */
    L3G4200D_MODE_800_50  = 0xe,        /**< data rate: 800Hz, cut-off: 50Hz */
    L3G4200D_MODE_800_110 = 0xf         /**< data rate: 800Hz, cut-off: 110Hz */
} l3g4200d_mode_t;

/**
 * @brief   Device initialization parameters
 */
typedef struct {
    i2c_t i2c;              /**< I2C device the sensor is connected to */
    uint8_t addr;           /**< the sensors slave address on the I2C bus */
    gpio_t int1;            /**< INT1 pin */
    gpio_t int2;            /**< INT2 (DRDY) pin */
    l3g4200d_mode_t mode;   /**< sampling frequency and bandwidth mode */
    l3g4200d_scale_t scale; /**< scaling factor to normalize results */
} l3g4200d_params_t;

/**
 * @brief   Device descriptor for L3G4200D sensors
 */
typedef struct {
    l3g4200d_params_t params;     /**< device initialization parameters */
    int32_t scale;                /**< internal scaling factor to normalize results */
} l3g4200d_t;

/**
 * @brief   Initialize a gyro
 *
 * @param[out] dev          device descriptor of sensor to initialize
 * @param[in]  params       initialization parameters
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int l3g4200d_init(l3g4200d_t *dev, const l3g4200d_params_t *params);

/**
 * @brief   Read angular speed value in degree per second from gyro
 *
 * @param[in]  dev          device descriptor of gyro
 * @param[out] acc_data     result vector in dps per axis
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int l3g4200d_read(const l3g4200d_t *dev, l3g4200d_data_t *acc_data);

/**
 * @brief   Power-up the given device
 *
 * @param[in]  dev          device to enable
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int l3g4200d_enable(const l3g4200d_t *dev);

/**
 * @brief   Power-down the given device
 *
 * @param[in]  dev          device to power-down
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int l3g4200d_disable(const l3g4200d_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* L3G4200D_H */
/** @} */
