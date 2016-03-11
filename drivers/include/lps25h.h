/*
 * Copyright (C) 2015 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    driver_lps25h LPS25H Pressure Sensor Driver
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

/**
 * @brief Device descriptor for LPS25H sensors
 */
typedef struct {
    i2c_t   i2c;                    /**< I2C device the sensor is connected to */
    uint8_t address;                /**< I2C bus address of the sensor */
} lps25h_t;

/**
 * @brief Possible sampling rates for LPS25H sensors
 */
typedef enum {
    LPS25H_ODR_1HZ   = 0x10,      /**< sample with 1Hz */
    LPS25H_ODR_7HZ   = 0x20,      /**< sample with 7Hz */
    LPS25H_ODR_12HZ5 = 0x30,      /**< sample with 12.5Hz */
    LPS25H_ODR_25HZ  = 0x40       /**< sample with 25Hz */
} lps25h_odr_t;

/**
 * @brief Initialize a given LPS25H pressure sensor
 *
 * @param[out]  dev     device descriptor of the sensor
 * @param[in]   i2c     I2C bus the sensor is connected to
 * @param[in]   address the sensor's address on the I2C bus
 * @param[in]   odr    internal sampling rate of the sensor
 *
 * @return               0 on success
 * @return              -1 on error
 */
int lps25h_init(lps25h_t *dev, i2c_t i2c, uint8_t address, lps25h_odr_t odr);

/**
 * @brief Read a temperature value from the given sensor, returnd in °C
 *
 * @param[in] dev       device descriptor of sensor to read from
 *
 * @return              temparature value in °C
 */
int lps25h_read_temp(lps25h_t *dev);

/**
 * @brief Read a pressure value from the given sensor, returned in mbar
 *
 * @param[in] dev       device descriptor of sensor to read from
 *
 * @return              pressure value in mbar
 */
int lps25h_read_pres(lps25h_t *dev);

/**
 * @brief Enable the given sensor
 *
 * @param[in] dev       device descriptor of sensor to enable
 *
 * @return               0 on success
 * @return              -1 on error
 */
int lps25h_enable(lps25h_t *dev);

/**
 * @brief Disable the given sensor
 *
 * @param[in] dev       device descriptor of sensor enable
 *
 * @return               0 on success
 * @return              -1 on success
 */
int lps25h_disable(lps25h_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* LPS25H_H */
/** @} */
