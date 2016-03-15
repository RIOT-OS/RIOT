/*
 * ng_sht11.h
 *
 *  Created on: Mar 14, 2016
 *      Author: dipswitch
 */

#ifndef DRIVERS_INCLUDE_NG_SHT11_H_
#define DRIVERS_INCLUDE_NG_SHT11_H_

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
} sht11_params_t;

typedef struct {
    i2c_t bus;              /**< SPI device the sensor is connected to */
    int16_t scale;          /**< Current scale setting of the sensor */
} sht11_t;

typedef struct {
    uint16_t temp;
    uint16_t rel;
} sht11_data_t;

int sht11_init(sht11_t *dev, i2c_t bus, uint8_t scale);

int sht11_read(const sht11_t *dev, sht11_data_t *data);

#ifdef __cplusplus
}
#endif

#endif /* DRIVERS_INCLUDE_NG_SHT11_H_ */
