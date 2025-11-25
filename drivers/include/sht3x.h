/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_sht3x
 * @brief       Device Driver for Sensirion SHT30/SHT31/SHT35 Humidity and Temperature Sensors
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 * @{
 */

#ifndef SHT3X_H
#define SHT3X_H

#include <stdint.h>
#include <stdbool.h>

#include "periph/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

/** possible I2C slave addresses */
#define SHT3X_I2C_ADDR_1    (0x44)  /**< ADDR pin connected to GND/VSS */
#define SHT3X_I2C_ADDR_2    (0x45)  /**< ADDR pin connected to VDD */

/**
 * @brief   Driver error codes (returned as negative values)
 */
typedef enum {
    SHT3X_OK,                       /**< no error */
    SHT3X_ERROR_I2C,                /**< I2C communication failure */
    SHT3X_ERROR_CRC,                /**< CRC check failed*/
    SHT3X_ERROR_STATUS,             /**< sensor has wrong status */
    SHT3X_ERROR_MEASURE_CMD_INV,    /**< measurement command not executed */
} sht3x_error_codes;

/**
 * @brief   SHT3x measurement modes
 */
typedef enum {
    SHT3X_SINGLE_SHOT = 0,  /**< single shot measurement */
    SHT3X_PERIODIC_0_5_MPS, /**< periodic with 0.5 measurements per second (mps) */
    SHT3X_PERIODIC_1_MPS,   /**< periodic with   1 measurements per second (mps) */
    SHT3X_PERIODIC_2_MPS,   /**< periodic with   2 measurements per second (mps) */
    SHT3X_PERIODIC_4_MPS,   /**< periodic with   4 measurements per second (mps) */
    SHT3X_PERIODIC_10_MPS   /**< periodic with  10 measurements per second (mps) */
} sht3x_mode_t;

/**
 * @brief   SHT3x repeatability levels
 *
 * The stated repeatability is 3 times the standard deviation of multiple
 * consecutive measurements at the stated repeatability and at constant
 * ambient conditions. It is a measure for the noise on the physical
 * sensor output. Different measurement modes allow for high/medium/low
 * repeatability. [Datasheet SHT3x-DIS]
 *
 * The configured repeatability level determines the time required for
 * a measurement:
 *
 * Reapeatability  | T typ.  | RH typ. | Maximum Measurement Duration
 * :---------------|:-------:|:-------:|:---------------------------:
 * high            | 0.04    | 0.08    | 15.5 ms
 * medium          | 0.08    | 0.15    | 6.5 ms
 * low             | 0.15    | 0.21    | 4.5 ms
 */
typedef enum {
    SHT3X_HIGH = 0, /**< high repeatability */
    SHT3X_MEDIUM,   /**< medium repeatability */
    SHT3X_LOW       /**< low repeatability */
} sht3x_repeat_t;

/**
 * @brief   SHT3x device initialization parameters
 */
typedef struct {
    i2c_t          i2c_dev;   /**< I2C device               */
    uint8_t        i2c_addr;  /**< I2C address              */
    sht3x_mode_t   mode;      /**< measurement mode used    */
    sht3x_repeat_t repeat;    /**< repeatability level used */
} sht3x_params_t;

/**
 * @brief   SHT3x sensor device data structure
 */
typedef struct {

    i2c_t           i2c_dev;         /**< I2C device  */
    uint8_t         i2c_addr;        /**< I2C address */

    sht3x_mode_t    mode;            /**< measurement mode used    */
    sht3x_repeat_t  repeat;          /**< repeatability level used */

    bool            meas_started;    /**< indicates whether measurement is started */
    uint32_t        meas_start_time; /**< start time of current measurement in us */
    uint32_t        meas_duration;   /**< time in us until the results of the
                                          current measurement become available */
} sht3x_dev_t;

/**
 * @brief	Initialize the SHT3x sensor device
 *
 * This function initializes the sensor device.
 *
 * If the sensor device is configured in any periodic measurement mode, it
 * immediately starts the periodic measurement with configured measurement
 * period and configured repeatability.
 *
 * In single-shot mode, the sensor device goes into the sleep mode to save
 * power. The single measurement is started when ::sht3x_read is called.
 *
 * @param[in]   dev     Device descriptor of SHT3x device to be initialized
 * @param[in]   params  SHT3x initialization parameters
 *
 * @return  0 on success or negative error code, see #sht3x_error_codes
 */
int sht3x_init (sht3x_dev_t *dev, const sht3x_params_t *params);

/**
 * @brief   Read SHT3x measurement results (temperature and humidity)
 *
 * The function returns the results of one measurement once they are available.
 *
 * If the single-shot measurement mode is configured, the function
 *
 * - starts the measurement according to the configured repeatability
 * - waits for the sensor to complete the measurement depending on the
 *   configured repeatability, and
 * - returns the measurement results.
 *
 * @note Since in single-shot measurement mode the measurement is only started
 * when this function is called, it delays the calling task up to 16 ms,
 * depending on the configured repeatability, in order to wait for the results
 * of the measurement.
 *
 * In the periodic measurement modes, the function returns the results of the
 * last periodic measurement, if already available.
 *
 * @note The ::sht3x_read function should be called less frequently than the
 * periodic measurements are performed. Otherwise, the function implicitly
 * delays the calling task until the results of the next measurement cycle
 * become available to avoid data underrun and sensor blocking.
 *
 * For either \p temp or \p hum also ```NULL``` can be passed, if only one
 * value is of interest.
 *
 * @param[in]   dev     Device descriptor of SHT3x device to read from
 * @param[out]  temp    Temperature in hundredths of a degree Celsius
 * @param[out]  hum     Relative Humidity in hundredths of a percent
 *
 * @return  0 on success or negative error code, see #sht3x_error_codes
 */
int sht3x_read (sht3x_dev_t* dev, int16_t* temp, int16_t* hum);

#ifdef __cplusplus
}
#endif

#endif /* SHT3X_H */
/** @} */
