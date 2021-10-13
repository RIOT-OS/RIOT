/*
 * Copyright (C) 2017 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup   drivers_adcxx1c ADCXX1C ADC device driver
 * @ingroup    drivers_sensors
 * @ingroup    drivers_saul
 * @brief      I2C Analog-to-Digital Converter device driver
 *
 * This driver works with adc081c, adc101c and adc121c versions.
 *
 * This driver provides @ref drivers_saul capabilities.
 * @{
 *
 * @file
 * @brief      ADCXX1C ADC device driver
 *
 * @author     Vincent Dupont <vincent@otakeys.com>
 */

#ifndef ADCXX1C_H
#define ADCXX1C_H

#ifdef __cplusplus
extern "C" {
#endif

#include "periph/i2c.h"
#include "periph/gpio.h"

/**
 * @brief  ADCxx1C default address (ADCxx1C021 address)
 */
#ifndef ADCXX1C_I2C_ADDRESS
#define ADCXX1C_I2C_ADDRESS  (0x54)
#endif

/**
 * @brief   ADC resolution
 */
enum {
    ADCXX1C_RES_8BITS  = 8,  /**< 8 bits resolution (ADC081C family) */
    ADCXX1C_RES_10BITS = 10, /**< 10 bits resolution (ADC101C family) */
    ADCXX1C_RES_12BITS = 12, /**< 12 bits resolution (ADC121C family) */
};

/**
 * @brief   ADC default resolution for device variants
 */
#if   defined(MODULE_ADC081C)
#define ADCXX1C_RES_DEFAULT     ADCXX1C_RES_8BITS
#elif defined(MODULE_ADC101C)
#define ADCXX1C_RES_DEFAULT     ADCXX1C_RES_10BITS
#elif defined(MODULE_ADC121C)
#define ADCXX1C_RES_DEFAULT     ADCXX1C_RES_12BITS
#else
#define ADCXX1C_RES_DEFAULT     (-1)
#error "ADCXX1C: Failed to select resolution: unknown ADCXX1C device variant!"
#endif

/**
 * @brief   Conversion interval configuration value
 */
enum {
    ADCXX1C_CYCLE_DISABLED = 0,  /**< No cycle conversion */
    ADCXX1C_CYCLE_32,            /**< Conversion cycle = Tconvert x 32 */
    ADCXX1C_CYCLE_64,            /**< Conversion cycle = Tconvert x 64 */
    ADCXX1C_CYCLE_128,           /**< Conversion cycle = Tconvert x 128 */
    ADCXX1C_CYCLE_256,           /**< Conversion cycle = Tconvert x 256 */
    ADCXX1C_CYCLE_512,           /**< Conversion cycle = Tconvert x 512 */
    ADCXX1C_CYCLE_1024,          /**< Conversion cycle = Tconvert x 1024 */
    ADCXX1C_CYCLE_2048,          /**< Conversion cycle = Tconvert x 2048 */
};

/**
 * @brief   Named return values
 */
enum {
    ADCXX1C_OK          =  0,       /**< everything was fine */
    ADCXX1C_NOI2C       = -1,       /**< I2C communication failed */
    ADCXX1C_NODEV       = -2,       /**< no ADCXX1C device found on the bus */
    ADCXX1C_NODATA      = -3        /**< no data available */
};

/**
 * @brief   Alert values
 */
enum {
    ADCXX1C_ALERT_UNDER_RANGE   = 1,    /**< Measured voltage fell below Vlow */
    ADCXX1C_ALERT_OVER_RANGE    = 2,    /**< Measured voltage exceeded Vhigh */
};

/**
 * @brief   ADCxx1C params
 */
typedef struct adcxx1c_params {
    i2c_t i2c;            /**< i2c device */
    uint8_t addr;         /**< i2c address */
    uint8_t bits;         /**< resolution */
    uint8_t cycle;        /**< conversion interval */
    gpio_t alert_pin;     /**< alert pin (GPIO_UNDEF if not connected) */
    int16_t low_limit;    /**< alert low value */
    int16_t high_limit;   /**< alert high value */
    int16_t hysteresis;   /**< alert hysteresis */
} adcxx1c_params_t;

/**
 * @brief   ADCxx1C alert callback
 */
typedef void (*adcxx1c_cb_t)(void *);

/**
 * @brief   ADCxx1C device descriptor
 */
typedef struct adcxx1c {
    adcxx1c_params_t params; /**< device driver configuration */
    adcxx1c_cb_t cb;         /**< alert callback */
    void *arg;               /**< alert callback param */
} adcxx1c_t;

/**
 * @brief   Initialize an ADCxx1C ADC device
 *
 * @param[in,out] dev  device descriptor
 * @param[in] params   device configuration
 *
 * @return zero on successful initialization, non zero on error
 */
int adcxx1c_init(adcxx1c_t *dev, const adcxx1c_params_t *params);

/**
 * @brief   Read a raw ADC value
 *
 * @param[in] dev   device descriptor
 * @param[out] raw  read value
 *
 * @return zero on successful read, non zero on error
 */
int adcxx1c_read_raw(const adcxx1c_t *dev, int16_t *raw);

/**
 * @brief   Enable alert interrupt
 *
 * @param[in] dev   device descriptor
 * @param[in] cb    callback called when the alert fires
 * @param[in] arg   callback argument
 *
 * @return zero on success, non zero on error
 */
int adcxx1c_enable_alert(adcxx1c_t *dev, adcxx1c_cb_t cb, void *arg);

/**
 * @brief   Set the alert parameters
 *
 * @param[in,out] dev      device descriptor
 * @param[in] low_limit    alert low limit
 * @param[in] high_limit   alert high limit
 * @param[in] hysteresis   alert hysteresis
 *
 * @return zero on success, non zero on error
 */
int adcxx1c_set_alert_parameters(const adcxx1c_t *dev, int16_t low_limit,
                                 int16_t high_limit, int16_t hysteresis);

/**
 * @brief   Get alert source and clear it
 *
 * @param[in,out]   dev       device descriptor
 *
 * @return a combination of ADCXX1C_ALERT_OVER_RANGE and ADCXX1C_ALERT_UNDER_RANGE on success
 * @return negative error code on error
 */
int adcxx1c_get_and_clear_alert(const adcxx1c_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* ADCXX1C_H */
/** @} */
