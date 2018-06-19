/*
 * Copyright (C) 2016 Kees Bakker, SODAQ
 *               2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_bmx280 BMP280/BME280 temperature, pressure and humidity sensor
 * @ingroup     drivers_sensors
 * @brief       Device driver interface for the Bosch BMP280 and BME280 sensors.
 *
 * BMP280 and BME280 measure temperature in centi °C and pressure in Pa. BME280
 * can also measure relative humidity in %.
 *
 * For more information, see the datasheets:
 * * [BMP280](https://ae-bst.resource.bosch.com/media/_tech/media/datasheets/BST-BMP280-DS001-18.pdf)
 * * [BME280](https://ae-bst.resource.bosch.com/media/_tech/media/datasheets/BST-BME280_DS001-11.pdf)
 *
 * @{
 * @file
 * @brief       Device driver interface for the BMX280 sensors (BMP280 and BME280).
 *
 * @details     There are three sensor values that can be read: temperature,
 *              pressure and humidity.  The BME280 device usually measures them
 *              all at once.  It is possible to skip measuring either of the
 *              values by changing the oversampling settings.  The driver is
 *              written in such a way that a measurement is only started from
 *              the function that reads the temperature.  In other words, you
 *              always have to call bme280_read_temperature, and then optionally
 *              you can call the other two.
 *
 * @author      Kees Bakker <kees@sodaq.com>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef BMX280_H
#define BMX280_H

#include <inttypes.h>
#include "saul.h"
#include "periph/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Calibration struct for the BMX280 sensor
 *
 * This must be read from the device at startup.
 */
typedef struct {
    uint16_t dig_T1;    /**< T1 coefficient */
    int16_t dig_T2;     /**< T2 coefficient */
    int16_t dig_T3;     /**< T3 coefficient */

    uint16_t dig_P1;    /**< P1 coefficient */
    int16_t dig_P2;     /**< P2 coefficient */
    int16_t dig_P3;     /**< P3 coefficient */
    int16_t dig_P4;     /**< P4 coefficient */
    int16_t dig_P5;     /**< P5 coefficient */
    int16_t dig_P6;     /**< P6 coefficient */
    int16_t dig_P7;     /**< P7 coefficient */
    int16_t dig_P8;     /**< P8 coefficient */
    int16_t dig_P9;     /**< P9 coefficient */

    uint8_t dig_H1;     /**< H1 coefficient */
    int16_t dig_H2;     /**< H2 coefficient */
    uint8_t dig_H3;     /**< H3 coefficient */
    int16_t dig_H4;     /**< H4 coefficient */
    int16_t dig_H5;     /**< H5 coefficient */
    int8_t dig_H6;      /**< H6 coefficient */
} bmx280_calibration_t;

/**
 * @brief   Values for t_sb field of the BMX280 config register
 */
typedef enum {
    BMX280_SB_0_5 = 0,
    BMX280_SB_62_5 = 1,
    BMX280_SB_125 = 2,
    BMX280_SB_250 = 3,
    BMX280_SB_500 = 4,
    BMX280_SB_1000 = 5,
    BMX280_SB_10 = 6,
    BMX280_SB_20 = 7
} bmx280_t_sb_t;

/**
 * @brief   Values for filter field of the BMX280 config register
 */
typedef enum {
    BMX280_FILTER_OFF = 0,
    BMX280_FILTER_2 = 1,
    BMX280_FILTER_4 = 2,
    BMX280_FILTER_8 = 3,
    BMX280_FILTER_16 = 4,
} bmx280_filter_t;

/**
 * @brief   Values for mode field of the BMX280 ctrl_meas register
 */
typedef enum {
    BMX280_MODE_SLEEP = 0,
    BMX280_MODE_FORCED = 1,
    BMX280_MODE_FORCED2 = 2,    /* Same as FORCED */
    BMX280_MODE_NORMAL = 3
} bmx280_mode_t;

/**
 * @brief   Values for oversampling settings
 *
 * These values are used for:
 *  - osrs_h field of the BME280 ctrl_hum register
 *  - osrs_t field of the BMX280 ctrl_meas register
 *  - osrs_p field of the BMX280 ctrl_meas register
 */
typedef enum {
    BMX280_OSRS_SKIPPED = 0,
    BMX280_OSRS_X1 = 1,
    BMX280_OSRS_X2 = 2,
    BMX280_OSRS_X4 = 3,
    BMX280_OSRS_X8 = 4,
    BMX280_OSRS_X16 = 5,
} bmx280_osrs_t;

/**
 * @brief   Parameters for the BMX280 sensor
 *
 * These parameters are needed to configure the device at startup.
 */
typedef struct {
    /* I2C details */
    i2c_t i2c_dev;                      /**< I2C device which is used */
    uint8_t i2c_addr;                   /**< I2C address */

    /* Config Register */
    bmx280_t_sb_t t_sb;                 /**< standby */
    bmx280_filter_t filter;             /**< filter coefficient */
    uint8_t spi3w_en;                   /**< Enables 3-wire SPI interface */

    /* ctrl_meas */
    bmx280_mode_t run_mode;             /**< ctrl_meas mode */
    bmx280_osrs_t temp_oversample;      /**< ctrl_meas osrs_t */
    bmx280_osrs_t press_oversample;     /**< ctrl_meas osrs_p */

    /* ctrl_hum */
    bmx280_osrs_t humid_oversample;     /**< ctrl_hum osrs_h */
} bmx280_params_t;

/**
 * @brief   Device descriptor for the BMX280 sensor
 */
typedef struct {
    bmx280_params_t params;             /**< Device Parameters */
    bmx280_calibration_t calibration;   /**< Calibration Data */
} bmx280_t;

/**
 * @brief   Status and error return codes
 */
enum {
    BMX280_OK           =  0,     /**< everything was fine */
    BMX280_ERR_NODEV    = -1,     /**< did not detect BME280 or BMP280 */
    BMX280_ERR_NOCAL    = -2,     /**< could not read calibration data */
};

/**
 * @brief   Initialize the given BMX280 device
 *
 * @param[out] dev          Initialized device descriptor of BMX280 device
 * @param[in]  params       The parameters for the BMX280 device (sampling rate, etc)
 *
 * @return                  BMX280_OK on success
 * @return                  BMX280_ERR_I2C
 * @return                  BMX280_ERR_NODEV
 * @return                  BMX280_ERR_NOCAL
 */
int bmx280_init(bmx280_t* dev, const bmx280_params_t* params);

/**
 * @brief   Read temperature value from the given BMX280 device, returned in centi °C
 *
 * @param[in] dev           Device descriptor of BMX280 device to read from
 *
 * @returns                 The temperature in centi Celsius. In case of an error
 *                          it returns INT16_MIN.
 */
int16_t bmx280_read_temperature(const bmx280_t* dev);

/**
 * @brief   Read air pressure value from the given BMX280 device, returned in PA
 *
 * @details This function should only be called after doing bmx280_read_temperature
 *          first.
 *
 * @param[in]  dev          Device descriptor of BMX280 device to read from
 *
 * @returns                 The air pressure in Pa
 */
uint32_t bmx280_read_pressure(const bmx280_t *dev);

#if defined(MODULE_BME280)
/**
 * @brief   Read humidity value from the given BME280 device, returned in centi %RH
 *
 * @details This function should only be called after doing bmx280_read_temperature
 *          first. It's only available with BME280 sensor.
 *
 * @param[in]  dev          Device descriptor of BME280 device to read from
 *
 * @returns                 Humidity in centi %RH (i.e. the percentage times 100)
 */
uint16_t bme280_read_humidity(const bmx280_t *dev);
#endif

#ifdef __cplusplus
}
#endif

#endif /* BMX280_H */
/** @} */
