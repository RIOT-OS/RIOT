/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_vcnl40x0 VCNL4010/VCNL4020/VCNL4040 Proximity and Ambient Light Sensors
 * @ingroup     drivers_sensors
 * @brief       Device driver interface for the VCNL40X0 Proximity and Ambient Light Sensors.
 * @{
 *
 * @file
 * @brief       Device driver interface for the VCNL40X0 sensors family.
 *
 * @note        VCNL4010, VCNL4020 and VCNL4040 are supported.
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef VCNL40X0_H
#define VCNL40X0_H

#include "saul.h"
#include "periph/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Proximity measure rate
 */
enum {
    VCNL40X0_PROXIMITY_RATE_2 = 0,          /**< 1.95 measurements/s (default) */
    VCNL40X0_PROXIMITY_RATE_4,              /**< 3.90625 measurements/s */
    VCNL40X0_PROXIMITY_RATE_8,              /**< 7.8125 measurements/s */
    VCNL40X0_PROXIMITY_RATE_16,             /**< 16.625 measurements/s */
    VCNL40X0_PROXIMITY_RATE_31,             /**< 31.25 measurements/s */
    VCNL40X0_PROXIMITY_RATE_62,             /**< 62.5 measurements/s */
    VCNL40X0_PROXIMITY_RATE_125,            /**< 125 measurements/s */
    VCNL40X0_PROXIMITY_RATE_250,            /**< 250 measurements/s */
};

/**
 * @brief Ambient light measurement rate
 */
enum {
    VCNL40X0_AMBIENT_RATE_1 = 0,            /**< 1 sample/s */
    VCNL40X0_AMBIENT_RATE_2,                /**< 2 sample/s (default) */
    VCNL40X0_AMBIENT_RATE_3,                /**< 3 sample/s */
    VCNL40X0_AMBIENT_RATE_4,                /**< 4 sample/s */
    VCNL40X0_AMBIENT_RATE_5,                /**< 5 sample/s */
    VCNL40X0_AMBIENT_RATE_6,                /**< 6 sample/s */
    VCNL40X0_AMBIENT_RATE_8,                /**< 8 sample/s */
    VCNL40X0_AMBIENT_RATE_10,               /**< 10 sample/s */
};

/**
 * @brief Ambient light number of conversions during one measurement cycle
 *
 * Number of conversions = 2^decimal value
 */
enum {
    VCNL40X0_AMBIENT_AVERAGE_1 = 0,         /**< Decimal value 1 */
    VCNL40X0_AMBIENT_AVERAGE_2,             /**< Decimal value 2 */
    VCNL40X0_AMBIENT_AVERAGE_4,             /**< Decimal value 4 */
    VCNL40X0_AMBIENT_AVERAGE_8,             /**< Decimal value 8 */
    VCNL40X0_AMBIENT_AVERAGE_16,            /**< Decimal value 16 */
    VCNL40X0_AMBIENT_AVERAGE_32,            /**< Decimal value 32 (default) */
    VCNL40X0_AMBIENT_AVERAGE_64,            /**< Decimal value 64 */
    VCNL40X0_AMBIENT_AVERAGE_128,           /**< Decimal value 128 */
};

/**
 * @brief   Status and error return codes
 */
enum {
    VCNL40X0_OK = 0,                        /**< Everything was fine */
    VCNL40X0_ERR_I2C,                       /**< Error initializing the I2C bus */
    VCNL40X0_ERR_NODEV                      /**< Error wrong device */
};

/**
 * @brief Device initialization parameters
 */
typedef struct {
    i2c_t i2c_dev;                          /**< I2C device which is used */
    uint8_t i2c_addr;                       /**< Address on I2C bus */
    uint8_t led_current;                    /**< LED current */
    uint8_t proximity_rate;                 /**< Proximity rate */
    uint8_t ambient_rate;                   /**< Ambient light rate */
    uint8_t ambient_avg;                    /**< Ambient light conversion average */
} vcnl40x0_params_t;

/**
 * @brief Device descriptor for the VCNL40X0 sensor
 */
typedef struct {
    vcnl40x0_params_t params;               /**< Device parameters */
} vcnl40x0_t;

/**
 * @brief Initialize the given VCNL40X0 device
 *
 * @param[out] dev          Initialized device descriptor of VCNL40X0 device
 * @param[in]  params       The parameters for the VCNL40X0 device
 *
 * @return                  VCNL40X0_OK on success
 * @return                  -VCNL40X0_ERR_I2C if given I2C is not enabled in board config
 * @return                  -VCNL40X0_ERR_NODEV if not a vcnl40X0 device
 */
int vcnl40x0_init(vcnl40x0_t *dev, const vcnl40x0_params_t *params);

/**
 * @brief Read proximity value from the vcnl40X0 device
 *
 * @param[in] dev           Device descriptor of VCNL40X0 device to read from
 *
 * @return                  Proximity in counts
 */
uint16_t vcnl40x0_read_proximity(const vcnl40x0_t *dev);

/**
 * @brief Read ambient light value from the vcnl40X0 device
 *
 * @param[in] dev           Device descriptor of VCNL40X0 device to read from
 *
 * @return                  Ambient light in counts
 */
uint16_t vcnl40x0_read_ambient_light(const vcnl40x0_t *dev);

/**
 * @brief Read illuminance value from the vcnl40X0 device
 *
 * @param[in] dev           Device descriptor of VCNL40X0 device to read from
 *
 * @return                  Illuminance in lux
 */
uint16_t vcnl40x0_read_illuminance(const vcnl40x0_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* VCNL40X0_H */
/** @} */
