/*
 * Copyright (C) 2016 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_bmp180 BMP180
 * @ingroup     drivers_sensors
 * @brief       Device driver interface for the BMP180 sensor
 * @{
 *
 * @file
 * @brief       Device driver interface for the BMP180 sensor.
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef BMP180_H
#define BMP180_H

#include "saul.h"
#include "periph/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Oversampling modes
 * @{
 */
#define BMP180_ULTRALOWPOWER          (0)
#define BMP180_STANDARD               (1)
#define BMP180_HIGHRES                (2)
#define BMP180_ULTRAHIGHRES           (3)
/** @} */

/**
 * @brief Calibration struct for the BMP180 sensor
 */
typedef struct {
    int16_t ac1;          /**< ac1 coefficient */
    int16_t ac2;          /**< ac2 coefficient */
    int16_t ac3;          /**< ac3 coefficient */
    int16_t b1;           /**< b1 coefficient */
    int16_t b2;           /**< b2 coefficient */
    int16_t mb;           /**< mb coefficient */
    int16_t mc;           /**< mc coefficient */
    int16_t md;           /**< md coefficient */
    uint16_t ac4;         /**< ac4 coefficient */
    uint16_t ac5;         /**< ac5 coefficient */
    uint16_t ac6;         /**< ac6 coefficient */
} bmp180_calibration_t;

/**
 * @brief Device descriptor for the BMP180 sensor
 */
typedef struct {
    i2c_t i2c_dev;                     /**< I2C device which is used */
    bmp180_calibration_t calibration;  /**< Device calibration */
    uint8_t oversampling;              /**< Oversampling mode */
} bmp180_t;


/**
 * @brief Device initialization parameters
 */
typedef struct {
    i2c_t i2c_dev;   /**< I2C device which is used */
    uint8_t mode;    /**< Oversampling mode */
} bmp180_params_t;

/**
 * @brief export SAUL endpoints
 * @{
 */
extern const saul_driver_t bmp180_temperature_saul_driver;
extern const saul_driver_t bmp180_pressure_saul_driver;
/** @} */

/**
 * @brief Initialize the given BMP180 device
 *
 * @param[out] dev          Initialized device descriptor of BMP180 device
 * @param[in]  i2c          I2C bus the sensor is connected to
 * @param[in]  mode         BMP180 oversampling mode
 *
 * @return                  0 on success
 * @return                  -1 if given I2C is not enabled in board config
 */
int bmp180_init(bmp180_t *dev, i2c_t i2c, uint8_t mode);

/**
 * @brief Read temperature value from the given BMP180 device, returned in d°C
 *
 * @param[in] dev           Device descriptor of BMP180 device to read from
 * @param[out] temperature  Temperature in d°C
 *
 * @return                  0 on success
 * @return                  -1 if device's I2C is not enabled in board config
 */
int bmp180_read_temperature(bmp180_t *dev, int32_t *temperature);

/**
 * @brief Read pressure value from the given BMP180 device, returned in Pa
 *
 * @param[in]  dev          Device descriptor of BMP180 device to read from
 * @param[out] pressure     Pressure in Pa
 *
 * @return                  0 on success
 * @return                  -1 if device's I2C is not enabled in board config
 */
int bmp180_read_pressure(bmp180_t *dev, int32_t *pressure);

/**
 * @brief Compute altitude, returned in m.
 *
 * @param[in]  dev          Device descriptor of BMP180 device to read from
 * @param[in]  pressure_0   The pressure at sea level in Pa
 * @param[out] altitude     Altitude in m
 *
 * @return                  0 on success
 * @return                  -1 if device's I2C is not enabled in board config
 */
int bmp180_altitude(bmp180_t *dev, int32_t pressure_0, int32_t *altitude);

/**
 * @brief Compute pressure at sea level, returned in Pa.
 *
 * @param[in]  dev          Device descriptor of BMP180 device to read from
 * @param[in]  altitude     Altitude in m
 * @param[out] pressure_0   Pressure at sea level in Pa
 *
 * @return                  0 on success
 * @return                  -1 if device's I2C is not enabled in board config
 */
int bmp180_sealevel_pressure(bmp180_t *dev, int32_t altitude, int32_t *pressure_0);

#ifdef __cplusplus
}
#endif

#endif /* BMP180_H */
/** @} */
