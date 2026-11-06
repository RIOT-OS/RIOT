/*
 * SPDX-FileCopyrightText: 2020 Puhang Ding
 * SPDX-FileCopyrightText: 2020 Jan Schlichter
 * SPDX-FileCopyrightText: 2020 Nishchay Agrawal
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    drivers_scd30 SCD30 CO2, temperature and humidity sensor
 * @ingroup     drivers_sensors
 * @{
 * @file
 * @brief       Device driver interface for the SCD30 sensor.
 *
 * @author      Nishchay Agrawal <f2016088@pilani.bits-pilani.ac.in>
 * @author      Puhang Ding      <czarsir@gmail.com>
 * @author      Jan Schlichter   <schlichter@ibr.cs.tu-bs.de>
 * @}
 */

#include "periph/i2c.h"
#include "saul.h"
#include "xtimer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    SCD30 Configuration parameter commands
 */
#define SCD30_VERSION    0xD100  /**< Get scd30 version */
#define SCD30_STATUS     0x0202  /**< Get data ready status from scd30
                                      device */

#define SCD30_DATA       0x0300  /**< Get data from scd30 device */

#define SCD30_START      0x0010  /**< Start measuring data from scd30 device */
#define SCD30_STOP       0x0104  /**< Stop measuring data from scd30 device */
#define SCD30_SOFT_RESET 0xD304  /**< Soft reset scd30 device */

#define SCD30_INTERVAL   0x4600  /**< Set measurement interval
                                      (2-1800 seconds) */
#define SCD30_ASC        0x5306  /**< De-Activate Automatic Self-Calibration */
#define SCD30_FRC        0x5204  /**< Forced Recalibration. 400-2000ppm */
#define SCD30_T_OFFSET   0x5403  /**< Set temperature Offset in 0.01 Celsius */
#define SCD30_A_OFFSET   0x5102  /**< Altitude Compensation in meters above
                                      sea level */

/**
 * @brief   Status and error codes for return values
 */
enum {
    SCD30_OK            =  0,
    SCD30_COM_FAILED    = -1,       /**< Communication with device failed. */
    SCD30_INVALID_VALUE = -2,       /**< Device doesn't exist. */
    SCD30_CRC_ERROR     = -3,       /**< Invalid value or length. */
    SCD30_NO_NEW_DATA   = -4,       /**< No new data. */
};

/**
 * @brief   Measurement from SCD30 sensor
 */
typedef struct {
    float co2_concentration;     /**< CO2 concentration in ppm */
    float temperature;            /**< Temperature measured in °C */
    float relative_humidity;     /**< Relative humidity measured in % */
} scd30_measurement_t;

/**
 * @brief   Device initialization parameters
 */
typedef struct {
    i2c_t i2c_dev;                  /**< I2C device which is used */
    uint8_t i2c_addr;               /**< I2C address */
} scd30_params_t;

/**
 * @brief   Device descriptor for the SCD30 sensor
 */
typedef struct {
    scd30_params_t params;          /**< Device initialization parameters */
} scd30_t;

/**
 *  @brief Initialize SCD30
 *
 *  @param dev         scd30 device
 *  @param params      scd30 device params
 *
 *  @return            SCD30_OK if device started
 */
int8_t scd30_init(scd30_t *dev, const scd30_params_t *params);

/**
 *  @brief Set a configuration parameter of device
 *
 *  @param dev         scd30 device
 *  @param param       param to be set/operation to be performed
 *                     param codes mentioned in scd30_internal
 *  @param val         value to be set for that parameter
 *
 *  @return            SCD30_OK on success
 */
int8_t scd30_set_param(const scd30_t *dev, uint16_t param, uint16_t val);

/**
 *  @brief Get value set for a configuration parameter on the device
 *
 *  @param dev         scd30 device
 *  @param param       param to be set/operation to be performed
 *                     param codes mentioned in scd30_internal
 *  @param val         Pointer to the value to be set for that parameter
 *
 *  @return            SCD30_OK on success
 */
int8_t scd30_get_param(scd30_t *dev, uint16_t param, uint16_t *val);

/**
 *  @brief  read CO2 concentration, temperature and relative humidity once
 *
 *  @param dev          scd30 device
 *  @param result       Values are stored in this struct
 *
 *  @return             SCD30_OK on success
 */
int8_t scd30_read_triggered(scd30_t *dev, scd30_measurement_t *result);

/**
 *  @brief read co2 concentration, temperature and relative humidity
 *         when continuous measurements are being taken
 *
 *  @param dev          scd30 device
 *  @param result       struct to store result
 *
 *  @return             SCD30_OK on success
 */
uint8_t scd30_read_periodic(scd30_t *dev, scd30_measurement_t *result);

/**
 *  @brief Initializes Continuous Measurements
 *
 *  @param dev          scd30 device
 *  @param interval     Interval at which new measurements have to be taken
 *                      in seconds (between 2 and 1800 seconds, both inclusive)
 *  @param apc          Average Pressure Compensation
 *                      0 to disable pressure compensation
 *                      700-1400 mBar for valid pressure compensation
 *
 *  @return             SCD30_OK if device started
 */
int scd30_start_periodic_measurement(scd30_t *dev, uint16_t *interval,
                                     uint16_t *apc);

/**
 *  @brief  Stop Continuous measurements
 *
 *  @param dev          scd30 dev device
 *
 *  @return             SCD30_OK if measurement stopped
 */
int8_t scd30_stop_measurements(const scd30_t *dev);

/**
 *  @brief  Soft reset sensor
 *
 *  @param dev          scd30 dev device
 *
 *  @return             SCD30_OK if soft reset successful
 */
int8_t scd30_reset(scd30_t *dev);

#ifdef __cplusplus
}
#endif
