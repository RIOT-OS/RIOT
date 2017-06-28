/*
 * Copyright (C) 2015 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_lps25h LPS25H Pressure Sensor Driver
 * @ingroup     drivers_sensors
 * @brief       Device driver for the LPS25H pressure sensor
 * @{
 *
 * @file
 * @brief       Device driver interface for the LPS25H pressure sensor
 *
 * @note        This driver uses the sensors I2C interface
 *
 * @author      René Herthel <rene-herthel@outlook.de>
 */

#ifndef LPS25H_H
#define LPS25H_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "periph/i2c.h"

/**
 * @brief The sensors default I2C address
 */
#define LPS25H_CHIP_ID        (0xBD)
#define LPS25H_CHIP_ADDR      (0x5E)

enum {
    LPS25H_OK,                 /**< All is OK */
    LPS25H_NOI2C,              /**< An error occurred during I2C initialization */
    LPS25H_ERRI2C,             /**< An error occurred with I2C */
    LPS25H_NODEV,              /**< Invalid device on I2C bus */
};

/**
 * @brief Possible output data rates for LPS25H sensors
 */
typedef enum {
    LPS25H_ODR_1HZ   = 0x10,   /**< sample with 1Hz */
    LPS25H_ODR_7HZ   = 0x20,   /**< sample with 7Hz */
    LPS25H_ODR_12HZ5 = 0x30,   /**< sample with 12.5Hz */
    LPS25H_ODR_25HZ  = 0x40    /**< sample with 25Hz */
} lps25h_odr_t;

/**
 * @brief Device parameters
 */
typedef struct {
    i2c_t   i2c;               /**< I2C device the sensor is connected to */
    uint8_t addr;              /**< I2C bus address of the sensor */
    lps25h_odr_t odr;          /**< Output data rate */
} lps25h_params_t;

/**
 * @brief Device descriptor for LPS25H sensors
 */
typedef struct {
    lps25h_params_t params;   /**< Device parameters */
} lps25h_t;

/**
 * @brief Initialize a given LPS25H pressure sensor
 *
 * @param[out]  dev     device descriptor of the sensor
 * @param[in]   params  device parameters
 *
 * @return              LPS25H_OK on success
 * @return              -LPS25H_NOI2C if no I2C is available
 * @return              -LPS25H_NOI2C when an I2C error occured
 * @return              -LPS25H_NODEV if no valid device is found
 */
int lps25h_init(lps25h_t *dev, const lps25h_params_t *params);

/**
 * @brief Read a temperature value from the given sensor, returned in °C
 *
 * @param[in] dev       device descriptor of sensor to read from
 *
 * @return              temparature value in °C
 */
int16_t lps25h_read_temperature(const lps25h_t *dev);

/**
 * @brief Read a pressure value from the given sensor, returned in mbar
 *
 * @param[in] dev       device descriptor of sensor to read from
 *
 * @return              pressure value in mbar
 */
uint16_t lps25h_read_pressure(const lps25h_t *dev);

/**
 * @brief Enable the given sensor
 *
 * @param[in] dev       device descriptor of sensor to enable
 */
void lps25h_enable(const lps25h_t *dev);

/**
 * @brief Disable the given sensor
 *
 * @param[in] dev       device descriptor of sensor enable
 */
void lps25h_disable(const lps25h_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* LPS25H_H */
/** @} */
