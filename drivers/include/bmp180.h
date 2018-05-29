/*
 * Copyright (C) 2016 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_bmp180 BMP180 temperature and pressure sensor
 * @ingroup     drivers_sensors
 * @brief       Device driver interface for the BMP180 sensor.
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
 * @brief   Oversampling modes
 */
typedef enum {
    BMP180_ULTRALOWPOWER = 0,
    BMP180_STANDARD,
    BMP180_HIGHRES,
    BMP180_ULTRAHIGHRES
} bmp180_oversampling_mode_t;

/**
 * @brief   Calibration struct for the BMP180 sensor
 */
typedef struct {
    int16_t ac1;                                /**< ac1 coefficient */
    int16_t ac2;                                /**< ac2 coefficient */
    int16_t ac3;                                /**< ac3 coefficient */
    int16_t b1;                                 /**< b1 coefficient */
    int16_t b2;                                 /**< b2 coefficient */
    int16_t mb;                                 /**< mb coefficient */
    int16_t mc;                                 /**< mc coefficient */
    int16_t md;                                 /**< md coefficient */
    uint16_t ac4;                               /**< ac4 coefficient */
    uint16_t ac5;                               /**< ac5 coefficient */
    uint16_t ac6;                               /**< ac6 coefficient */
} bmp180_calibration_t;

/**
 * @brief   Device initialization parameters
 */
typedef struct {
    i2c_t i2c_dev;                              /**< I2C device which is used */
    uint8_t i2c_addr;                           /**< I2C address */
    bmp180_oversampling_mode_t oversampling;    /**< Oversampling mode */
} bmp180_params_t;

/**
 * @brief   Device descriptor for the BMP180 sensor
 */
typedef struct {
    bmp180_params_t      params;                /**< Device initialization parameters */
    bmp180_calibration_t calibration;           /**< Device calibration */
} bmp180_t;

/**
 * @brief   Status and error return codes
 */
enum {
    BMP180_OK = 0,                              /**< everything was fine */
    BMP180_ERR_NODEV,                           /**< did not detect BMP180 */
    BMP180_ERR_NOCAL,                           /**< error when reading calibration values */
};

/**
 * @brief   Initialize the given BMP180 device
 *
 * @param[out] dev          Initialized device descriptor of BMP180 device
 * @param[in]  params       Initialization parameters
 *
 * @return                  BMP180_OK on success
 * @return                  -BMP180_ERR_NODEV if not a BMP180 at given address
 * @return                  -BMP180_ERR_NOCAL if an error occured when reading calibration values
 */
int bmp180_init(bmp180_t *dev, const bmp180_params_t *params);

/**
 * @brief   Read temperature value from the given BMP180 device, returned in d°C
 *
 * @param[in] dev           Device descriptor of BMP180 device to read from
 *
 * @return                  Temperature in d°C
 */
int16_t bmp180_read_temperature(const bmp180_t *dev);

/**
 * @brief   Read pressure value from the given BMP180 device, returned in Pa
 *
 * @param[in]  dev          Device descriptor of BMP180 device to read from
 *
 * @return                  Pressure in Pa
 */
uint32_t bmp180_read_pressure(const bmp180_t *dev);

/**
 * @brief   Compute altitude, returned in m.
 *
 * @param[in]  dev          Device descriptor of BMP180 device to read from
 * @param[in]  pressure_0   The pressure at sea level in Pa
 *
 * @return                  Altitude in m
 */
int16_t bmp180_altitude(const bmp180_t *dev, uint32_t pressure_0);

/**
 * @brief   Compute pressure at sea level, returned in Pa.
 *
 * @param[in]  dev          Device descriptor of BMP180 device to read from
 * @param[in]  altitude     Altitude in m
 *
 * @return                  Pressure at sea level in Pa
 */
uint32_t bmp180_sealevel_pressure(const bmp180_t *dev, int16_t altitude);

#ifdef __cplusplus
}
#endif

#endif /* BMP180_H */
/** @} */
