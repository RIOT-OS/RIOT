/*
 * Copyright 2015 Ludwig Ortmann, Christian Mehlis
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    driver_dht DHT Family of Humidity and Temperature Sensors
 * @ingroup     drivers
 * @brief       Device driver for the DHT Family of humidity
 *              and temperature sensors
 *
 * @{
 *
 * @file
 * @brief       Device driver interface for the DHT family of humidity
 *              and temperature sensors
 *
 * @author      Ludwig Ortmann <ludwig.ortmann@fu-berlin.de
 * @author      Christian Mehlis <mehlis@inf.fu-berlin.de>
 */

#ifndef DHT_H
#define DHT_H

#include <stdint.h>

#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief data type for storing DHT sensor readings
 */
typedef struct {
    uint16_t humidity;      /**< relative deca-humidity */
    uint16_t temperature;   /**< temperature in deca-Celsius */
} dht_data_t;

/**
 * @brief device type of the DHT device
 */
typedef enum {
    DHT11,                  /**< DHT11 device identifier */
    DHT22                   /**< DHT22 device identifier */
} dht_type_t;

/**
 * @brief device descriptor for DHT sensor devices
 */
typedef struct {
    gpio_t gpio;            /**< GPIO pin of the device's data pin */
    dht_type_t type;        /**< type of the DHT device */
} dht_t;

/**
 * @brief initialize a new DHT device
 *
 * @param[in] dev       device descriptor of a DHT device
 * @param[in] type      type of the DHT device
 * @param[in] gpio      GPIO pin the device's data pin is connected to
 *
 * @return              0 on success
 * @return              -1 on error
 */
int dht_init(dht_t *dev, dht_type_t type, gpio_t gpio);

 /**
 * @brief   read sensor data from device
 *
 * @param[in]  dev      device descriptor of a DHT device
 * @param[out] relhum   pointer to relative humidity in g/m^3
 * @param[out] temp     pointer to temperature in degrees Celsius
 *
 * @return              0 on success
 * @return              -1 on error
 */
int dht_read(dht_t *dev, float *relhum, float *temp);

 /**
 * @brief   read sensor data from device
 *
 * @note    When reading fails and the function indicates an error,
 *          data will contain garbage.
 *
 * @param[in] dev       device descriptor of a DHT device
 * @param[in] data      pointer to DHT data object
 *
 * @return              0 on success
 * @return              -1 on checksum error
 */
int dht_read_raw(dht_t *dev, dht_data_t *data);

/**
 * @brief   parse raw sensor data into relative humidity and Celsius
 *
 * @param[in]   dev     device descriptor of a DHT device
 * @param[in]   data    sensor data
 * @param[out]  relhum  pointer to relative humidity in g/m^3
 * @param[out]  temp    pointer to temperature in degrees Celsius
 */
void dht_parse(dht_t *dev, dht_data_t *data, float *relhum, float *temp);

#ifdef __cplusplus
}
#endif

#endif /* DHT_H */
/** @} */
