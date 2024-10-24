/*
 * Copyright (C) 2021 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_lm75
 *
 * @{
 * @file
 * @brief       Driver for the LM75 temperature sensor.
 *
 * @author      Vitor Batista <vitor.batista@ml-pa.com>
 *
 */

/**
 * @defgroup    drivers_lm75     LM75 Temperature Sensor driver compile configuration
 * @ingroup     drivers_sensors
 * @brief       Driver for the lm75 temperature sensors.
 */

#ifndef LM75_H
#define LM75_H

#include <stdbool.h>
#include "periph/i2c.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name LM75 return values
 */
enum {
    LM75_SUCCESS = 0,
    LM75_ERROR_I2C,
    LM75_ERROR
};

/**
 * @brief temperature reading properties/resolutions struct of the LM75 sensors
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
 * @brief params required for initialization
 */
typedef struct lm75_params {
    const    lm75_properties_t *res;      /**< Temperature resolutions */
    uint16_t conv_rate;                   /**< Conversion rate in ms */
    gpio_t   gpio_alarm;                  /**< Over-temperature alarm */
    i2c_t    i2c_bus;                     /**< I2C Bus used */
    uint8_t  i2c_addr;                    /**< i2c address */
    uint8_t  shutdown_mode;               /**< Shutdown mode register */
    uint8_t  tm_mode;                     /**< Thermistor Mode */
    uint8_t  polarity;                    /**< OS polarity register */
    uint8_t  fault_q;                     /**< Fault Queue register */
    /* only configurable for the TMP1075 */
    uint8_t  conv_rate_reg;               /**< Device Conversion rate register */
} lm75_params_t;

/**
 * @brief lm75 device descriptor
 */
typedef struct lm75 {
    lm75_params_t    lm75_params; /**< Configuration Parameters */
} lm75_t;

/**
 * @brief Initialization of the LM75 sensor
 *
 * Initializes the sensor according to specific input parameters.
 *
 * @param[out] dev        device structure to initialize
 * @param[in] params      initialization parameters
 *
 * @return LM75_SUCCESS, on success
 * @return LM75_ERROR_I2C, on I2C related error
 * @return LM75_ERROR, on initialization related error
 */
int lm75_init(lm75_t *dev, const lm75_params_t *params);

/**
 * @brief Temperature values of LM75 sensor
 *
 * Reads the sensor temperature values from TEMP_REG
 * the value is given with the full precision the device is capable of
 * If divided by the device's mult property, the result will be the temperature in ºC
 * and the remainder of that division will be the decimal part of the temperature,
 * at the maximum resolution the device is capable of.
 *
 * @param[in] dev                device structure
 * @param[in] temperature        buffer where temperature value will be written
 *
 * @return LM75_SUCCESS, on success
 * @return LM75_ERROR_I2C, on I2C related error
 */
int lm75_get_temperature_raw(lm75_t *dev, int *temperature);

/**
 * @brief Temperature values of LM75 sensor
 *
 * Gets the device's temperature register with the lm75_get_temperature_raw
 * function and then returns the values in mºC, truncating values smaller than this, if available
 *
 * @param[in] dev                device structure
 * @param[in] temperature        buffer where temperature value will be written in mºC
 *
 * @return LM75_SUCCESS, on success
 * @return LM75_ERROR, on error
 */
int lm75_get_temperature(lm75_t *dev, int *temperature);

/**
 * @brief Sets the values for Overtemperature shutdown(OS) and Hysteresis temperature(HYST).
 * OS gives the temperature's higher bound and HYST the lower bound
 * values are rounded to the lowest value that the device supports,
 *
 * @param[in] dev           device structure
 * @param[in] temp_os       desired OS temperature in mºC
 * @param[in] temp_hyst     desired HYST temperature in mºC
 * @param[in] cb            callback that is called from interrupt context
 * @param[in] *arg          optional arguments for the gpio_init_int function
 *
 * @return LM75_SUCCESS, on success
 * @return LM75_ERROR_I2C, on I2C related error
 * @return LM75_ERROR, on temperature setting related error
 */
int lm75_set_temp_limits(lm75_t *dev, int temp_hyst, int temp_os, gpio_cb_t cb, void *arg);

/**
 * @brief Overshutdown temperature value of LM75 sensor
 *
 * Reads the sensor OS temperature value from TOS_REG in ºC.
 *
 * @param[in] dev                 device structure
 * @param[out] temperature        buffer where OS temperature value will be written
 *
 * @return LM75_SUCCESS, on success
 * @return LM75_ERROR_I2C, on I2C related error
 */
int lm75_get_os_temp(lm75_t *dev, int *temperature);

/**
 * @brief Hysteresis temperature value of LM75 sensor
 *
 * Reads the sensor hysteresis temperature value from THYST_REG in ºC.
 *
 * @param[in] dev                 device structure
 * @param[out] temperature        buffer where HYST temperature value will be written
 *
 * @return LM75_SUCCESS, on success
 * @return LM75_ERROR_I2C, on I2C related error
 */
int lm75_get_hyst_temp(lm75_t *dev, int *temperature);

/**
 * @brief Read the current state of the OS pin to see if it's active.
 *
 * Read the configuration register to see the OS pin's polarity and
 * then reads its state. Then outputs if the pin is active and whether
 * it's in the low and active or high and active.
 *
 * @param[in] dev               device structure
 * @param[out] os_pin_state     pointer to the state of the OS pin - 0 for inactive and 1 for active
 *
 * @return LM75_SUCCESS, on success
 * @return LM75_ERROR, on pin reading error
 */
int lm75_get_os_pin(lm75_t *dev, bool *os_pin_state);

/**
 * @brief Activate the LM75 sensor shutdown mode
 *
 * @param[in] dev      device structure to set into shutdown mode
 *
 * @return LM75_SUCCESS, on success
 * @return LM75_ERROR, on mode switching related error
 */
int lm75_poweroff(lm75_t *dev);

/**
 * @brief Deactivate the LM75 sensor shutdown mode
 *
 * @param[in] dev      device structure to wake up from shutdown mode
 *
 * @return LM75_SUCCESS, on success
 * @return LM75_ERROR, on mode switching related error
 */
int lm75_poweron(lm75_t *dev);

/**
 * @brief Activates one shot conversion mode
 *
 * Wakes from shutdown mode, does a single temperature conversion
 * and writes in into the temperature register and then goes back into shutdown
 *
 * @param[in] dev       device structure
 *
 * @return LM75_ERROR if unsuccessful
 * @return LM75_SUCCESS if successful
 */
int tmp1075_one_shot(lm75_t *dev);

/**
 * @brief Activates low power mode operation
 *
 * This function makes the device measure temperatures in a strictly discrete way
 * at a user definable rate, as opposed to performing continuous measurements
 * at the device's conversion rate.
 * It allows the device to stay in shutdown mode for the most part, therefore consuming less power.
 * In the tmp1075 and other devices which have the one shot feature this is done automatically.
 * In the LM75A sensor nd other sensors which lack the one shot mode feature this is done manually
 * by switching the device to and from shutdown mode and staying awake at least long enough
 * to perform one ne measurement.
 *
 * @param[in] dev           device structure
 * @param[in] interval      time interval in ms between measurements
 *
 * @return LM75_ERROR if unsuccessful
 * @return LM75_SUCCESS if successful
 */
int lm75_low_power_mode(lm75_t *dev, uint16_t interval);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* LM75_H */
