/*
 * ng_sht11.h
 *
 *  Created on: Mar 14, 2016
 *      Author: dipswitch
 */

#ifndef DRIVERS_INCLUDE_M24LRXXX_H_
#define DRIVERS_INCLUDE_M24LRXXX_H_

#include <stdint.h>

#include "periph/i2c.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Device descriptor for SHT11 sensors
 */
typedef struct {
    i2c_t bus;              /**< I2C device the sensor is connected to */
    i2c_speed_t speed;      /**< I2C bus speed we use to communicate to the device */
    uint8_t address;        /**< I2C address of the device */
} m24lrxxx_params_t;

typedef struct {
    i2c_t bus;              /**< I2C device the sensor is connected to */
    i2c_speed_t speed;      /**< I2C bus speed we use to communicate to the device */
    uint8_t address;        /**< I2C address of the device */
} m24lrxxx_t;

int m24lrxxx_init(m24lrxxx_t *dev, i2c_t bus, i2c_speed_t speed, uint8_t address);

int m24lrxxx_read(const m24lrxxx_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* DRIVERS_INCLUDE_M24LRXXX_H_ */
