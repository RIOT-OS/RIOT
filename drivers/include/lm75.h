/*
 * SPDX-FileCopyrightText: 2021 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_lm75
 *
 * @{
 * @file
 * @brief       Driver for the LM75 and TMP1075 temperature sensors.
 *
 * @author      Vitor Batista <vitor.batista@ml-pa.com>
 *
 */

/**
 * @defgroup    drivers_lm75     LM75 and TMP1075 Temperature Sensor Driver
 * @ingroup     drivers_sensors
 * @brief       Driver for the LM75 and TMP1075 temperature sensors.
 */

#include <errno.h> /* IWYU pragma: keep needed for EIO etc. */
#include <stdbool.h>

#include "periph/gpio.h"
#include "periph/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name        Backwards compatible LM75 return values
 * @deprecated  Use `errno` directly instead
 * @{
 */
#define LM75_SUCCESS   0         /**< Success */
#define LM75_ERROR_I2C (-EIO)    /**< I2C communication error */
#define LM75_ERROR     (-EINVAL) /**< Other error */
/** @} */

/**
 * @brief Temperature reading properties/resolutions struct of the LM75 sensors
 */
typedef struct lm75_properties {
    uint16_t    os_res;      /**< resolution of the OS and HYST registers */
    uint16_t    os_mult;     /**< multiplier required for getting the OS and HYST into ºC */
    uint16_t    temp_res;    /**< resolution of the temperature register */
    uint16_t    temp_mult;   /**< multiplier required for getting the temperature into ºC */
    uint8_t     os_shift;    /**< how many bits need to be shifted (2 bytes - any unused bits) */
    uint8_t     temp_shift;  /**< how many bits need to be shifted (2 bytes - any unused bits) */
} lm75_properties_t;

extern lm75_properties_t lm75a_properties;      /**< declaration present in lm75.c */
extern lm75_properties_t tmp1075_properties;    /**< declaration present in lm75.c */

/**
 * @brief Parameters required for initialization
 */
typedef struct lm75_params {
    const    lm75_properties_t *res;      /**< Temperature resolutions */
    /**
     * @brief   Device Conversion Rate register
     *
     * @warning Only configurable for the TM1075 and ignored for other devices
     */
    uint16_t conv_rate;
    gpio_t   gpio_alarm;                  /**< Over-temperature alarm */
    i2c_t    i2c_bus;                     /**< I2C Bus used */
    uint8_t  i2c_addr;                    /**< I2C address */
    uint8_t  shutdown_mode;               /**< Shutdown mode register */
    uint8_t  tm_mode;                     /**< Thermistor Mode */
    uint8_t  polarity;                    /**< OS polarity register */
    uint8_t  fault_q;                     /**< Fault Queue register */
    /* only configurable for the TMP1075 */
    uint8_t  conv_rate_reg;               /**< Device Conversion rate register */
} lm75_params_t;

/**
 * @brief LM75 device descriptor
 */
typedef struct lm75 {
    lm75_params_t    lm75_params; /**< Configuration Parameters */
} lm75_t;

/**
 * @brief Initialization of the LM75 sensor
 *
 * Initializes the sensor according to the specified input parameters.
 *
 * @param[out]  dev         device structure to initialize
 * @param[in]   params      initialization parameters
 *
 * @retval      0           success
 * @retval      -EINVAL     Not an LM75 or TMP1075 sensor (value in ID register
 *                          does not match)
 * @retval      <0          Error from I2C, see @ref i2c_read_reg
 */
int lm75_init(lm75_t *dev, const lm75_params_t *params);

/**
 * @brief Temperature values of LM75 sensor
 *
 * Reads the sensor temperature values from TEMP_REG. The value is given with
 * the full precision the device is capable of. If divided by the device's
 * temp_mult property, the result will be the temperature in ºC and the
 * remainder of that division will be the decimal part of the temperature, at
 * the maximum resolution the device is capable of.
 *
 * @param[in]   dev         device structure
 * @param[out]  temperature buffer that the temperature value will be written to
 *
 * @retval      0           success
 * @retval      <0          Error from I2C, see @ref i2c_read_reg
 */
int lm75_get_temperature_raw(const lm75_t *dev, int32_t *temperature);

/**
 * @brief Temperature values of LM75 sensor
 *
 * Gets the device's temperature register with the @ref lm75_get_temperature_raw
 * function and then returns the values in mºC, truncating values smaller than
 * this, if available.
 *
 * @param[in]   dev         device structure
 * @param[out]  temperature that the temperature value will be written to in mºC
 *
 * @retval      0           success
 * @retval      <0          Error from I2C, see @ref i2c_read_reg
 */
int lm75_get_temperature(const lm75_t *dev, int32_t *temperature);

/**
 * @brief Sets the values for Overtemperature Shutdown (OS) and Hysteresis
 *        Temperature (HYST).
 *
 * OS gives the temperature's upper bound and HYST the lower bound.
 * The values are rounded to the lowest value that the device supports.
 *
 * @param[in]   dev         device structure
 * @param[in]   temp_hyst   desired HYST temperature in mºC
 * @param[in]   temp_os     desired OS temperature in mºC
 * @param[in]   cb          callback that is called from interrupt context
 * @param[in]   arg         optional arguments for the @ref gpio_init_int function
 *
 * @retval      0           success
 * @retval      -EINVAL     Invalid hysteresis or invalid GPIO config
 * @retval      <0          Error from I2C, see @ref i2c_read_reg
 */
int lm75_set_temp_limits(const lm75_t *dev, int32_t temp_hyst, int32_t temp_os,
                         gpio_cb_t cb, void *arg);

/**
 * @brief Overtemperature Shutdown (OS) value of the LM75 sensor
 *
 * Reads the sensor OS temperature value from TOS_REG in ºC.
 *
 * @param[in]   dev         device structure
 * @param[out]  temperature buffer that the OS temperature value will be written to
 *
 * @retval      0           success
 * @retval      <0          Error from I2C, see @ref i2c_read_reg
 */
int lm75_get_os_temp(const lm75_t *dev, int32_t *temperature);

/**
 * @brief Hysteresis Temperature (HYST) value of the LM75 sensor
 *
 * Reads the sensor hysteresis temperature value from THYST_REG in ºC.
 *
 * @param[in]   dev         device structure
 * @param[out]  temperature buffer that the HYST temperature value will be written to
 *
 * @retval      0           success
 * @retval      <0          Error from I2C, see @ref i2c_read_reg
 */
int lm75_get_hyst_temp(const lm75_t *dev, int32_t *temperature);

/**
 * @brief Read the current state of the OS pin to see if it's active.
 *
 * Reads the configuration register to see the OS pin's polarity and
 * then reads its state. Then outputs if the pin is active and whether
 * it's in the low and active or high and active.
 *
 * @param[in]   dev             device structure
 * @param[out]  os_pin_state    pointer to the state of the OS pin - 0 for
 *                              inactive and 1 for active
 *
 * @retval      0               success
 * @retval      <0              Error from I2C, see @ref i2c_read_reg
 */
int lm75_get_os_pin(const lm75_t *dev, bool *os_pin_state);

/**
 * @brief Activate the LM75 sensor shutdown mode
 *
 * @param[in]   dev         device structure to set into shutdown mode
 *
 * @retval      0           success
 * @retval      <0          Error from I2C, see @ref i2c_read_reg
 */
int lm75_poweroff(const lm75_t *dev);

/**
 * @brief Deactivate the LM75 sensor shutdown mode
 *
 * @param[in]   dev         device structure to wake up from shutdown mode
 *
 * @retval      0           success
 * @retval      <0          Error from I2C, see @ref i2c_read_reg
 */
int lm75_poweron(const lm75_t *dev);

/**
 * @brief Activates one shot conversion mode
 *
 * Wakes from shutdown mode, does a single temperature conversion
 * and writes in into the temperature register and then goes back into shutdown.
 *
 * @param[in]   dev         device structure
 *
 * @retval      0           success
 * @retval      -ENOTSUP    feature not supported by hardware variant used
 * @retval      <0          Error from I2C, see @ref i2c_read_reg
 */
int tmp1075_one_shot(const lm75_t *dev);

/**
 * @brief Activates low power mode operation
 *
 * This function makes the device measure temperatures in a strictly
 * discrete way at a user definable rate, as opposed to performing continuous
 * measurements at the device's conversion rate.
 *
 * It allows the device to stay in shutdown mode for the most part, therefore
 * consuming less power. In the TMP1075 and other devices which have the one
 * shot feature this is done automatically. In the LM75A sensor and other
 * sensors which lack the one shot mode feature this is done manually by
 * switching the device to and from shutdown mode and staying awake at least
 * long enough to perform one measurement.
 *
 * @param[in]   dev         device structure
 * @param[in]   interval    time interval in ms between measurements
 *
 * @retval      0           success
 * @retval      <0          Error from I2C, see @ref i2c_read_reg
 */
int lm75_low_power_mode(const lm75_t *dev, uint16_t interval);

#ifdef __cplusplus
}
#endif

/** @} */
