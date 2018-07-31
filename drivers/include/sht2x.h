/*
 * Copyright (C) 2017 George Psimenos (gp7g14@soton.ac.uk)
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_sht2x SHT2x Digital Humidity & Temperature Sensor driver
 * @ingroup     drivers_sensors
 * @brief       Driver for the Sensirion digital SHT2x Humidity & Temperature
 *              sensor series. This driver can initialize the sensor in any of
 *              its available resolutions. Humidity & temperature measurements
 *              are performed in no-hold mode (see datasheet), to ensure
 *              non-blocking operations. 
 *
 * @{
 *
 * @file
 * @brief       API for the SHT2x sensor driver.
 *
 * @author      George Psimenos <gp7g14@soton.ac.uk>
 */

#ifndef SHT2X_H
#define SHT2X_H

#include <stdint.h>
#include "periph/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef SHT2X_I2C_ADDRESS
#define SHT2X_I2C_ADDRESS           0x40 /**< Default address */
#endif

/**
 * @brief   Named return values
 */
enum {
    SHT2X_OK          =  0,       /**< everything was fine */
    SHT2X_DATA_READY  =  1,       /**< new data ready to be read */
    SHT2X_NOI2C       = -1,       /**< I2C communication failed */
    SHT2X_NODEV       = -2,       /**< no SHT2X device found on the bus */
    SHT2X_NODATA      = -3,       /**< no data available */
    SHT2X_READERROR   = -999,     /**< I2C read error */
    SHT2X_OTHERERROR  = -998      /**< fatal error */
};

/**
 * @brief   Devices supported by this driver
 */
enum {
    SHT2X_TYPE_SHT20 = 0x01,  /**< SHT20 */
    SHT2X_TYPE_SHT21 = 0x02,  /**< SHT21 */
    SHT2X_TYPE_SHT25 = 0x03   /**< SHT25 */
};

/**
 * @brief   Available resolutions
 */
enum {
    SHT2x_RES_12_14BIT = 0x00, /**< RH=12bit, T=14bit */
    SHT2x_RES_8_12BIT = 0x01,  /**< RH= 8bit, T=12bit */
    SHT2x_RES_10_13BIT = 0x80, /**< RH=10bit, T=13bit */
    SHT2x_RES_11_11BIT = 0x81, /**< RH=11bit, T=11bit */
};

/**
 * @brief   Configuration parameters
 */
typedef struct {
    i2c_t i2c;                  /**< I2C bus the device is connected to */
    uint8_t addr;               /**< I2C bus address of the device */
    uint8_t type;               /**< device type */
    uint8_t resolution;         /**< desired resolution */
} sht2x_params_t;

/**
 * @brief   Device descriptor
 */
typedef struct {
    sht2x_params_t params;    /**< device configuration parameters */
} sht2x_t;


/**
 * @brief   Initialize the SHT2x sensor driver.
 *
 * @param[out] dev          device descriptor of sensor to initialize
 * @param[in]  params       configuration parameters
 *
 * @return                  SHT2X_OK on success
 * @return                  SHT2X_NOI2C if initialization of I2C bus failed
 * @return                  SHT2X_NODEV if sensor test failed
 */
int sht2x_init(sht2x_t *dev, const sht2x_params_t *params);

/**
 * @brief   Read temperature from the SHT2x sensor.
 *
 * @param[in] dev           device descriptor of sensor to read
 *
 * @return                  Temperature in hundredths of °C
 * @return                  SHT2X_NODEV if sensor communication failed
 * @return                  SHT2X_OTHERERROR if parameters are invalid
 * @return                  SHT2X_READERROR if sensor times out
 */
int16_t sht2x_read_temp(sht2x_t *dev);

/**
 * @brief   Read humidity from the SHT2x sensor.
 *
 * @param[in] dev           device descriptor of sensor to read
 *
 * @return                  Humidity in hundredths of RH%
 * @return                  SHT2X_NODEV if sensor communication failed
 * @return                  SHT2X_OTHERERROR if parameters are invalid
 * @return                  SHT2X_READERROR if sensor times out
 */
int16_t sht2x_read_humidity(sht2x_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* SHT2X_H */
/** @} */
