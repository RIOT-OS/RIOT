/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_htu21d HTU21D humidity and temperature
 * @ingroup     drivers_sensors
 * @ingroup     drivers_saul
 * @brief       Device driver for HTU21D humidity and temperature sensor
 *
 * The driver works also with Sensirion SHT21.
 *
 * The sensor only supports single measurements and polling. Therefore,
 * the measurement process includes the following steps:
 *
 * 1. Trigger the sensor to measure either temperature or relative humidity.
 *    This is done by #htu21d_measure_tmp or #htu21d_measure_hum.
 * 2. Wait the time the sensor requires to complete the measurement.
 *    The maximum time in ms the sensor might require is returned by
 *    #htu21d_measure_tmp and #htu21d_measure_hum. The measurement
 *    time depends on the configured resolution, see #htu21d_res_t.
 * 3. Fetch the results for either temperature or relative humidity
 *    with #htu21d_fetch_tmp or #htu21d_fetch_hum.
 *
 * @note
 * These steps can be performed either for temperature or humidity only,
 * but not for both at the same time. That is, the measurements can not
 * be mixed:
 * - After triggering a temperature measurement with
 *   the #htu21d_measure_tmp function, the #htu21d_fetch_tmp function
 *   must be used to retrieve the results.
 * - After triggering a humidity measurement with
 *   the #htu21d_measure_hum function, the #htu21d_fetch_hum function
 *   must be used to retrieve the results.
 * Otherwise, incorrect results will be retrieved.
 *
 * Example:
 * ```
 * while (1) {
 *     ...
 *     int res;
 *     int16_t tmp;
 *     int16_t raw;
 *
 *     res = htu21d_measure_tmp(&dev);
 *     if (res > 0) {
 *         xtimer_usleep(res * US_PER_MS);
 *     }
 *     res = htu21d_fetch_tmp (&dev, &tmp, &raw);
 *       ...
 * }
 * ```
 *
 * The #htu21d_fetch_tmp and #htu21d_fetch_hum return the raw sensor data
 * as 16-bit 2's complement and the sensor data converted to temperature
 * or humidity.
 *
 * If any data are not needed, the corresponding parameter can be set to
 * NULL, for example
 *
 * ```
 *     res = htu21d_fetch_tmp (&dev, NULL, &raw);
 * ```
 * to fetch only raw sensor data for the temperature.
 *
 * For convenience, the #htu21d_read function carries out all measurement
 * steps for temperature and/or humidity as one blocking function call. Using
 * As with other fetch functions, the parameters can be set to NULL for
 * data that are not needed.
 *
 * @note Using this function can be faster because #htu21d_measure_tmp and
 * #htu21d_measure_hum return the maximum time the sensor needs according to
 * the datasheet. This maximum time can be up to 85 ms. The # htu21d_read
 * functoin checks the availability of the results every 5 ms and may
 * be faster if the sensor does not need the maximum time.
 *
 * Example:
 * ```
 * while (1) {
 *     ...
 *     int res;
 *     int16_t tmp;
 *     int16_t hum_raw;
 *
 *     res = htu21d_read(&dev, &tmp, NULL, NULL, &hum_raw);
 *     ...
 * }
 * ```
 *
 * Although the sensor supports a so-called hold-master mode that uses
 * clock stretching to keep the master waiting when a measurement is
 * triggered until the results are avialablae, this hold-master mode
 * is not used by the driver
 *
 * - to prevent the master from being blocked,
 * - to avoid blocking the I2C bus, and
 * - not all I2C implementations support clock stretching.
 *
 * This driver provides @ref drivers_saul capabilities.
 *
 * @{
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 */

#ifndef HTU21D_H
#define HTU21D_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stdint.h>

#include "periph/gpio.h"
#include "periph/i2c.h"

/** HTU21D I2C address */
#define HTU21D_I2C_ADDRESS  (0x40)

/** Definition of error codes */
typedef enum {
    HTU21D_OK,                   /**< success */
    HTU21D_ERROR_I2C,            /**< I2C communication error */
    HTU21D_ERROR_I2C_NO_ACK,     /**< I2C no ack from slave */
    HTU21D_ERROR_CRC,            /**< data CRC error */
    HTU21D_ERROR_MEASURING,      /**< measurement in progress */
    HTU21D_ERROR_NOT_MEASURING,  /**< measurement not started  */
} htu21d_error_codes_t;

/**
 * @brief   Resolutions
 *
 * The resolution determines the maximum time needed by the sensor for
 * one measurement.
 *
 * Resolution | RH [ms] | T [ms]
 * ---------- | ------- | ------
 * 14 bit     |         | 85
 * 13 bit     |         | 43
 * 12 bit     | 29      | 22
 * 11 bit     | 15      | 11
 * 10 bit     |  9      ||
 *  8 bit     |  4      ||
 */
typedef enum {
    HTU21D_RES_RH12_T14 = 0,    /**< RH with 12 bit, Temperature with 14 */
    HTU21D_RES_RH8_T12,         /**< RH with  8 bit, Temperature with 12 */
    HTU21D_RES_RH10_T13,        /**< RH with 13 bit, Temperature with 13 */
    HTU21D_RES_RH11_T11,        /**< RH with 11 bit, Temperature with 11 */
} htu21d_res_t;

/**
 * @brief   HTU21D device initialization parameters
 */
typedef struct {
    unsigned  dev;   /**< I2C device (default I2C_DEV(0)) */
    uint8_t   addr;  /**< I2C slave address (default #HTU21D_I2C_ADDRESS) */
    htu21d_res_t res; /**< measurement resolution (default #HTU21D_RES_RH12_T14) */
} htu21d_params_t;

/**
 * @brief   HTU21D sensor device data structure type
 */
typedef struct {
    bool measure_tmp;          /**< measurement of temperature started */
    bool measure_hum;          /**< measurement of humidity started */
    htu21d_params_t params;    /**< device initialization parameters */

} htu21d_t;

/**
 * @brief	Initialize the HTU21D sensor device
 *
 * This function resets the sensor and initializes the sensor according to
 * given intialization parameters. All registers are reset to default values.
 *
 * @param[in]   dev     device descriptor of HTU21D sensor to be initialized
 * @param[in]   params  HTU21D initialization parameters
 *
 * @retval  HTU21D_OK      on success
 * @retval  HTU21D_ERROR_* a negative error code on error,
 *                         see #htu21d_error_codes_t
 */
int htu21d_init(htu21d_t *dev, const htu21d_params_t *params);

/**
 * @brief   Trigger a single measurement of temperature
 *
 * Triggers a single measurement of the temperature and returns the time
 * in ms needed by the sensor to finish the measurement. Temperature can be
 * fetched after this time using function #htu21d_fetch_tmp.
 *
 * @param[in]   dev     device descriptor of HTU21D sensor
 *
 * @return   time needed by the sensor on success, or
 *           a negative error code on error, see #htu21d_error_codes_t
 */
int htu21d_measure_tmp (htu21d_t *dev);

/**
 * @brief   Trigger a single measurement of relative humidity
 *
 * Triggers a single measurement of the relative humidity and returns the
 * time in ms needed by the sensor to finish the measurement. Relative
 * humidity Temperature can be fetched after this time using
 * function #htu21d_fetch_tmp.
 *
 * @param[in]   dev     device descriptor of HTU21D sensor
 *
 * @return   time needed by the sensor on success, or
 *           a negative error code on error, see #htu21d_error_codes_t
 */
int htu21d_measure_hum (htu21d_t *dev);

/**
 * @brief   Fetch the temperature in hundredths of a degree Celsius
 *
 * The function reads the raw sensor data from the measurement previously
 * started with #htu21d_measure_tmp and converts them to the temperature in
 * hundredths of a degree Celsius. If the measurement has not been finished
 * when it is called, it fails with the error code #HTU21D_ERROR_MEASURING.
 * The error code can be used for polling.
 *
 * @note The \p tmp and \p raw parameter can be NULL if values are not
 * of interest.
 *
 * @param[in]   dev     device descriptor of HTU21D sensor
 * @param[out]  tmp     temperature in hundredths of a degree Celsius
 * @param[out]  raw     raw temperature sensor data as 16 bit two's complement
 *
 * @retval  HTU21D_OK                   on success
 * @retval  HTU21D_ERROR_MEASURING      measurement still in progress
 * @retval  HTU21D_ERROR_NOT_MEASURING  measurement not started
 * @retval  HTU21D_ERROR_*              a negative error code on error,
 *                                      see #htu21d_error_codes_t
 */
int htu21d_fetch_tmp (htu21d_t *dev, int16_t *tmp, int16_t *raw);

/**
 * @brief   Fetch the relative humidity in hundredths of a degree Celsius
 *
 * The function reads raw sensor data from the measurement previously
 * started with #htu21d_measure_hum and converts them to the relative
 * humidity hundredths of a percent. If the measurement has not been finished
 * when it is called, it fails with the error code #HTU21D_ERROR_MEASURING.
 * The error code can be used for polling.
 *
 * @param[in]   dev     device descriptor of HTU21D sensor
 * @param[out]  hum     relative humidity in hundredths of a percent
 * @param[out]  raw     raw humidity sensor data as 16 bit two's complement
 *
 * @note The \p hum and \p raw parameter can be NULL if values are not
 * of interest.
 *
 * @retval  HTU21D_OK                   on success
 * @retval  HTU21D_ERROR_MEASURING      measurement still in progress
 * @retval  HTU21D_ERROR_NOT_MEASURING  measurement not started
 * @retval  HTU21D_ERROR_*              a negative error code on error,
 *                                      see #htu21d_error_codes_t
 */
int htu21d_fetch_hum (htu21d_t *dev, int16_t *hum, int16_t *raw);

/**
 * @brief   Convenience function to trigger a measurement and fetcht the results
 *
 * The function triggers a single measurement of temperature and/or relative
 * humidity and returns the converted measurement results.
 *
 * Output parameters can be NULL if values are not of interest.
 *
 * @note This function waits for measurement results and returns once these
 * results are available. Depending on the resolution, it can take from some
 * ms up to 85 ms.
 *
 * @param[in]   dev     device descriptor of HTU21D sensor
 * @param[out]  tmp     temperature in hundredths of a degree Celsius
 * @param[out]  hum     relative humidity in hundredths of a percent
 * @param[out]  raw_tmp raw temperature sensor data as 16 bit two's complement
 * @param[out]  raw_hum raw humidity sensor data as 16 bit two's complement
 *
 * @retval  HTU21D_OK       on success
 * @retval  HTU21D_ERROR_*  a negative error code on error,
 *                          see #htu21d_error_codes_t
 */
int htu21d_read (htu21d_t *dev,
                 int16_t *tmp, int16_t *hum,
                 int16_t *raw_tmp, int16_t *raw_hum);

#ifdef __cplusplus
}
#endif

#endif /* HTU21D_H */
/** @} */
