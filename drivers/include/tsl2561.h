/*
 * Copyright (C) 2016 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_tsl2561 TSL2561 illuminance sensor
 * @ingroup     drivers_sensors
 * @ingroup     drivers_saul
 * @brief       Device driver interface for the illuminance TSL2561 sensor
 *
 * This driver provides @ref drivers_saul capabilities.
 * @{
 *
 * @file
 * @brief       Device driver interface for the illuminance TSL2561 sensor.
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef TSL2561_H
#define TSL2561_H

#include "saul.h"
#include "periph/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    TSL2561 I2C addresses
 * @{
 */
#define TSL2561_ADDR_LOW                  (0x29)
#define TSL2561_ADDR_FLOAT                (0x39)
#define TSL2561_ADDR_HIGH                 (0x49)
/** @} */

/**
 * @name    TSL2561 integration times
 * @{
 */
#define TSL2561_INTEGRATIONTIME_13MS      (0x00)    /* 13.7ms */
#define TSL2561_INTEGRATIONTIME_101MS     (0x01)    /* 101ms  */
#define TSL2561_INTEGRATIONTIME_402MS     (0x02)    /* 402ms  */
#define TSL2561_INTEGRATIONTIME_NA        (0x03)    /* N/A    */
/** @} */

/**
 * @name    TSL2561 gains
 * @{
 */
#define TSL2561_GAIN_1X                   (0x00)
#define TSL2561_GAIN_16X                  (0x10)
/** @} */

/**
 * @name    TSL2561 driver initialization return codes
 * @{
 */
#define TSL2561_OK                        (0)
#define TSL2561_NOI2C                     (-1)
#define TSL2561_BADDEV                    (-2)
/** @} */

/**
 * @brief   Device initialization parameters
 */
typedef struct {
    i2c_t i2c_dev;                     /**< I2C device which is used */
    uint8_t addr;                      /**< address on I2C bus */
    uint8_t gain;                      /**< gain */
    uint8_t integration;               /**< integration time */
} tsl2561_params_t;

/**
 * @brief   Device descriptor for the TSL2561 sensor
 */
typedef struct {
    tsl2561_params_t params;           /**< device initialization parameters */
} tsl2561_t;

/**
 * @brief   Initialize the given TSL2561 device
 *
 * @param[out] dev          Initialized device descriptor of BMP180 device
 * @param[in]  params       Initialization parameters
 *
 * @return                  0 on success
 * @return                  -1 if given I2C is not available
 * @return                  -2 if not a TSL2561 sensor
 */
int tsl2561_init(tsl2561_t *dev, const tsl2561_params_t *params);

/**
 * @brief   Read illuminance value from the given TSL2561 device, returned in lx.
 *
 * @param[in]  dev          Device descriptor of TSL2561 device to read from
 *
 * @return                  Illuminance in Lux (lx)
 */
uint16_t tsl2561_read_illuminance(const tsl2561_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* TSL2561_H */
/** @} */
