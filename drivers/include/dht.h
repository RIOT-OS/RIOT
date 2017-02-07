/*
 * Copyright 2015 Ludwig Knüpfer,
 *           2015 Christian Mehlis
 *           2016-2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_dht DHT Family of Humidity and Temperature Sensors
 * @ingroup     drivers_sensors
 * @brief       Device driver for the DHT Family of humidity
 *              and temperature sensors
 *
 * @{
 *
 * @file
 * @brief       Device driver interface for the DHT family of humidity
 *              and temperature sensors
 *
 * @author      Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de
 * @author      Christian Mehlis <mehlis@inf.fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef DHT_H
#define DHT_H

#include <stdint.h>

#include "saul.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief possible return codes
 */
enum {
    DHT_OK      =  0,       /**< all good */
    DHT_NOCSUM  = -1,       /**< checksum error */
    DHT_NODEV   = -2        /**< device type not defined */
};

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
    DHT22,                  /**< DHT22 device identifier */
    DHT21 = DHT22           /**< DHT21 device identifier */
} dht_type_t;

/**
 * @brief device descriptor for DHT sensor devices
 */
typedef struct {
    gpio_t pin;             /**< GPIO pin of the device's data pin */
    dht_type_t type;        /**< type of the DHT device */
    gpio_mode_t in_mode;    /**< input pin configuration, with or without pull
                             *   resistor */
} dht_t;

/**
 * @brief configuration parameters for DHT devices
 */
typedef dht_t dht_params_t;

/**
 * @brief export SAUL endpoints
 * @{
 */
extern const saul_driver_t dht_temp_saul_driver;
extern const saul_driver_t dht_hum_saul_driver;
/** @} */

/**
 * @brief auto-initialize all configured DHT devices
 */
void dht_auto_init(void);

/**
 * @brief initialize a new DHT device
 *
 * @param[out] dev      device descriptor of a DHT device
 * @param[in]  params   configuration parameters
 *
 * @return              0 on success
 * @return              -1 on error
 */
int dht_init(dht_t *dev, const dht_params_t *params);

/**
 * @brief   get a new temperature and humidity value from the device
 *
 * @note    if reading fails or checksum is invalid, no new values will be
 *          written into the result values
 *
 * @param[in]  dev      device descriptor of a DHT device
 * @param[out] temp     temperature value [in °C * 10^-1]
 * @param[out] hum      relative humidity value [in percent * 10^-1]
 *
 * @return              0 on success
 * @return              -1 on checksum error
 * @return              -2 on parsing error
 */
int dht_read(dht_t *dev, int16_t *temp, int16_t *hum);

#ifdef __cplusplus
}
#endif

#endif /* DHT_H */
/** @} */
