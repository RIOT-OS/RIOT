/*
 * Copyright (C) 2014 PHYTEC Messtechnik GmbH
 *               2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_hdc1000 HDC1000 Humidity and Temperature Sensor
 * @ingroup     drivers_sensors
 * @brief       Driver for the TI HDC1000 Humidity and Temperature Sensor
 *
 * The driver will initialize the sensor for best resolution (14 bit). Currently
 * the driver doesn't use the heater. Temperature and humidity are acquired in
 * sequence. The sensor is always in sleep mode.
 *
 * The temperature and humidity values can either be acquired using the
 * simplified `hdc1000_read()` function, or the conversion can be triggered
 * manually using the `hdc1000_trigger_conversion()` and `hdc1000_get_results()`
 * functions sequentially. If using the second method, on must wait at least
 * `HDC1000_CONVERSION_TIME` between triggering the conversion and reading the
 * results.
 *
 * @note        The driver does currently not support using the devices heating
 *              unit.
 *
 * @{
 *
 * @file
 * @brief       Interface definition for the HDC1000 sensor driver
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef HDC1000_H
#define HDC1000_H

#include <stdint.h>

#include "periph/i2c.h"
#include "hdc1000_regs.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief   Default I2C bus address of HDC1000 devices
 */
#ifndef HDC1000_I2C_ADDRESS
#define HDC1000_I2C_ADDRESS           (0x43)
#endif

/**
 * @brief   Typical conversion time needed to acquire new values [in us]
 *
 * @note    This time value is chosen twice as large as needed for two 14-bit
 *          conversions (worst case) to allow for timer imprecision:
 *          (convert temp + convert hum) * 2 -> (6.5ms + 6.5ms) * 2 := 26ms.
 */
#ifndef HDC1000_CONVERSION_TIME
#define HDC1000_CONVERSION_TIME       (26000)
#endif

/**
 * @brief   HDC1000 specific return values
 */
enum {
    HDC1000_OK     = 0,     /**< everything went as expected */
    HDC1000_NODEV  = -1,    /**< no HDC1000 device found on the bus */
    HDC1000_NOBUS  = -2,    /**< errors while initializing the I2C bus */
    HDC1000_BUSERR = -3     /**< error during I2C communication */
};

/**
 * @brief   Possible resolution values
 */
typedef enum {
    HDC1000_11BIT = (HDC1000_TRES11 | HDC1000_HRES11),  /**< 11-bit conversion */
    HDC1000_14BIT = (HDC1000_TRES14 | HDC1000_HRES14)   /**< 14-bit conversion */
} hdc1000_res_t;

/**
 * @brief   Parameters needed for device initialization
 */
typedef struct {
    i2c_t i2c;               /**< bus the device is connected to */
    uint8_t addr;            /**< address on that bus */
    hdc1000_res_t res;       /**< resolution used for sampling temp and hum */
    uint32_t renew_interval; /**< interval for cache renewal */
} hdc1000_params_t;

/**
 * @brief   Device descriptor for HDC1000 sensors
 */
typedef struct {
    hdc1000_params_t p;     /**< Configuration parameters */
} hdc1000_t;

/**
 * @brief   Initialize the given HDC1000 device
 *
 * @param[out] dev          device descriptor of sensor to initialize
 * @param[in]  params       configuration parameters
 *
 * @return                  HDC1000_OK on success
 * @return                  HDC1000_NOBUS if initialization of I2C bus fails
 * @return                  HDC1000_NODEV if no HDC1000 device found on bus
 */
int hdc1000_init(hdc1000_t *dev, const hdc1000_params_t *params);

/**
 * @brief   Trigger a new conversion
 *
 * After the conversion is triggered, one has to wait
 * @ref HDC1000_CONVERSION_TIME us until the results can be read using
 * @ref hdc1000_get_results().
 *
 * @param[in]  dev          device descriptor of sensor
 *
 * @return                  HDC1000_OK on success
 * @return                  HDC1000_BUSERR on I2C communication failures
 */
int hdc1000_trigger_conversion(const hdc1000_t *dev);

/**
 * @brief   Read conversion results for temperature and humidity
 *
 * @param[in]  dev          device descriptor of sensor
 * @param[out] temp         temperature [in 100 * degree centigrade]
 * @param[out] hum          humidity [in 100 * percent relative]
 *
 * @return                  HDC1000_OK on success
 * @return                  HDC1000_BUSERR on I2C communication failures
 */
int hdc1000_get_results(const hdc1000_t *dev, int16_t *temp, int16_t *hum);

/**
 * @brief   Convenience function for reading temperature and humidity
 *
 * This function will trigger a new conversion, wait for the conversion to be
 * finished and the get the results from the device.
 *
 * @param[in]  dev          device descriptor of sensor
 * @param[out] temp         temperature [in 100 * degree centigrade]
 * @param[out] hum          humidity [in 100 * percent relative]
 *
 * @return                  HDC1000_OK on success
 * @return                  HDC1000_BUSERR on I2C communication failures
 */
int hdc1000_read(const hdc1000_t *dev, int16_t *temp, int16_t *hum);

/**
 * @brief   Extended read function including caching capability
 *
 * This function will return cached values if they are within the sampling
 * period (HDC1000_RENEW_INTERVAL), or will trigger a new conversion, wait for
 * the conversion to be finished and the get the results from the device.
 *
 * @param[in]  dev          device descriptor of sensor
 * @param[out] temp         temperature [in 100 * degree centigrade]
 * @param[out] hum          humidity [in 100 * percent relative]
 *
 * @return                  HDC1000_OK on success
 * @return                  HDC1000_BUSERR on I2C communication failures
 */
int hdc1000_read_cached(const hdc1000_t *dev, int16_t *temp, int16_t *hum);

#ifdef __cplusplus
}
#endif

#endif /* HDC1000_H */
/** @} */
