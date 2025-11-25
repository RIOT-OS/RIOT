/*
 * Copyright (C) 2014 Freie Universität Berlin
 *               2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_lpsxxx LPS331AP/LPS25HB/LPS22HB Pressure Sensors Driver
 * @ingroup     drivers_sensors
 * @ingroup     drivers_saul
 * @brief       Device driver for the LPSXXX pressure sensor family (LPS331AP/LPS25HB/LPS22HB)
 *
 * This driver provides @ref drivers_saul capabilities.
 *
 * @note This driver uses the sensors I2C interface
 *
 * @{
 *
 * @file
 *
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef LPSXXX_H
#define LPSXXX_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "periph/i2c.h"

/**
 * @defgroup drivers_lpsxxx_config     LPS331AP/LPS25HB/LPS22HB/LPS22HH/LPS22CH driver compile configuration
 * @ingroup config_drivers_sensors
 * @{
 */
/**
 * @brief   The sensors default I2C address
 *
 * The address depends on the status of SDO/SA0 Pin. Default
 * address corresponds to SDO/SA0 connected to VDD. For more
 * information refer to the section 'I2C operation' in the
 * datasheet.
 */
#ifndef CONFIG_LPSXXX_DEFAULT_ADDRESS
#define CONFIG_LPSXXX_DEFAULT_ADDRESS  (0x5d)
#endif
/** @} */

/**
 * @brief   Return codes
 */
enum {
    LPSXXX_OK,                  /**< Everything was fine */
    LPSXXX_ERR_NODEV,           /**< No valid device found */
    LPSXXX_ERR_I2C,             /**< An error occurred on the I2C bus */
};

/**
 * @brief   Possible sampling rates for LPS331AP sensors
 */
typedef enum {
    LPSXXX_RATE_1HZ = 1,        /**< sample with 1Hz */
#if MODULE_LPS331AP
    LPSXXX_RATE_7HZ = 5,        /**< sample with 7Hz, default */
    LPSXXX_RATE_12HZ5 = 6,      /**< sample with 12.5Hz */
    LPSXXX_RATE_25HZ = 7        /**< sample with 25Hz */
#elif MODULE_LPS25HB
    LPSXXX_RATE_7HZ = 2,        /**< sample with 7Hz, default */
    LPSXXX_RATE_12HZ5 = 3,      /**< sample with 12.5Hz */
    LPSXXX_RATE_25HZ = 4        /**< sample with 25Hz */
#elif MODULE_LPS22HB
    LPSXXX_RATE_10HZ = 2,       /**< sample with 10Hz */
    LPSXXX_RATE_25HZ = 3,       /**< sample with 25Hz, default */
    LPSXXX_RATE_50HZ = 4,       /**< sample with 50Hz */
    LPSXXX_RATE_75HZ = 5        /**< sample with 75Hz */
#elif MODULE_LPS22HH || MODULE_LPS22CH
    LPSXXX_RATE_10HZ = 2,       /**< sample with 10Hz */
    LPSXXX_RATE_25HZ = 3,       /**< sample with 25Hz, default */
    LPSXXX_RATE_50HZ = 4,       /**< sample with 50Hz */
    LPSXXX_RATE_75HZ = 5,       /**< sample with 75Hz */
    LPSXXX_RATE_100HZ = 6,      /**< sample with 100Hz */
    LPSXXX_RATE_200HZ = 7       /**< sample with 200Hz */
#endif
} lpsxxx_rate_t;

/**
 * @brief   The sensors default output data rate (ODR)
 */
#if MODULE_LPS331AP || MODULE_LPS25HB
#define LPSXXX_DEFAULT_RATE     (LPSXXX_RATE_7HZ)
#else /* MODULE_LPS22HB || MODULE_LPS22HH  || MODULE_LPS22CH */
#define LPSXXX_DEFAULT_RATE     (LPSXXX_RATE_25HZ)
#endif

/**
 * @brief   Struct holding all parameters needed for device initialization
 */
typedef struct {
    i2c_t i2c;                  /**< I2C bus the sensor is connected to */
    uint8_t addr;               /**< the devices address on the bus */
    lpsxxx_rate_t rate;         /**< tell sensor to sample with this rate */
} lpsxxx_params_t;

/**
 * @brief   Device descriptor for LPSXXX sensors
 */
typedef struct {
    lpsxxx_params_t params;     /**< device initialization parameters */
} lpsxxx_t;

/**
 * @brief   Initialize a given LPSXXX pressure sensor
 *
 * @param[out] dev      device descriptor of the sensor
 * @param[in]  params   initialization parameters
 *
 * @return              LPSXXX_OK on success
 * @return              -LPSXXX_ERR_NODEV if no valid device found
 * @return              -LPSXXX_ERR_I2C on I2C error
 */
int lpsxxx_init(lpsxxx_t *dev, const lpsxxx_params_t *params);

/**
 * @brief   Read a temperature value from the given sensor, returned in c°C
 *
 * @param[in] dev       device descriptor of sensor to read from
 * @param[out] temp     temperature value in c°C
 *
 * @return              LPSXXX_OK on success
 * @return              -LPSXXX_ERR_I2C on I2C error
 */
int lpsxxx_read_temp(const lpsxxx_t *dev, int16_t *temp);

/**
 * @brief   Read a pressure value from the given sensor, returned in hPa
 *
 * @param[in] dev       device descriptor of sensor to read from
 * @param[out] pres     pressure value in hPa
 *
 * @return              LPSXXX_OK on success
 * @return              -LPSXXX_ERR_I2C on I2C error
 */
int lpsxxx_read_pres(const lpsxxx_t *dev, uint16_t *pres);

/**
 * @brief   Enable the given sensor
 *
 * @param[in] dev       device descriptor of sensor to enable
 *
 * @return              LPSXXX_OK on success
 * @return              -LPSXXX_ERR_I2C on I2C error
 */
int lpsxxx_enable(const lpsxxx_t *dev);

/**
 * @brief   Disable the given sensor
 *
 * @param[in] dev       device descriptor of sensor to disable
 *
 * @return              LPSXXX_OK on success
 * @return              -LPSXXX_ERR_I2C on I2C error
 */
int lpsxxx_disable(const lpsxxx_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* LPSXXX_H */
/** @} */
