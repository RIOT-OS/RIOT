/*
 * Copyright 2017, RWTH Aachen. All rights reserved.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_shtc1 SHTC1 Temperature and humidity sensor
 * @ingroup     drivers_sensors
 * @ingroup     drivers_saul
 * @brief       Driver for the Sensirion SHTC1 sensor.
 *
 * @file
 * @brief       Device driver interface for the SHTC1 Temperature and humidity sensor
 *
 * @author      Steffen Robertz <steffen.robertz@rwth-aachen.de>
 * @author      Josua Arndt <jarndt@ias.rwth-aachen.de>
 */

#ifndef SHTC1_H
#define SHTC1_H

#include <stdint.h>
#include "saul.h"

#include "periph/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   SHTC1 Default Address
 *
 */
#ifndef SHTC1_I2C_ADDRESS
#define SHTC1_I2C_ADDRESS         (0x70)
#endif

/**
 * @brief   settings struct with all relevant parameters
 * @{
 */
typedef struct {
    i2c_t i2c_dev;              /**< I2C bus descriptor. */
    uint8_t i2c_addr;           /**< I2C address of the sensor. */
} shtc1_params_t;
/** @} */

/**
 * @brief   device descriptor for the SHTC1
 * @{
 */
typedef struct {
    shtc1_params_t params;  /**< Parameters struct with all settings set. */
} shtc1_t;
/** @} */

enum {
    SHTC1_OK,           /**< Success, no error */
    SHTC1_ERROR_BUS,    /**< I2C bus error */
    SHTC1_ERROR_CRC,    /**< CRC error */
    SHTC1_ERROR         /**< General error */
};

/**
 * @brief Initializes the sensor and I2C.
 *
 * @param[in] dev       I2C device descriptor.
 * @param[in] params    SHTC1 parameters to be used.
 *
 * @return              SHTC1_OK on a working initialization.
 * @return              SHTC1_ERROR_BUS Reading I2C failed.
 * @return              SHTC1_ERROR_CRC Wrong ID.
 */
int8_t shtc1_init(shtc1_t* const dev, const shtc1_params_t* params);

/**
 * @brief Reads all register values from the device.
 * @details The values as raw data will be saved into reveived.
 *
 * @param[in] dev             The I2C device descriptor.
 * @param[in] rel_humidity    Humidity in centi %.
 * @param[in] temperature     Temperature in centi °C.
 *
 * @return              SHTC1_OK if a measurement completed.
 * @return              SHTC1_ERROR_BUS reading I2C failed.
 * @return              SHTC1_ERROR_CRC on checksum error.
 */
int8_t shtc1_read(const shtc1_t *dev, uint16_t *rel_humidity, int16_t *temperature);

/**
 * @brief Reads the ID and saves it in the device descriptor
 *
 * @details             When working correctly ID should equal xxxx'xxxx'xx00'0111 where x is unspecified.
 *
 * @param[in] dev       The I2C device descriptor.
 * @param[in] id        ID of the device.
 *
 * @return              SHTC1_OK on everything done.
 * @return              SHTC1_ERROR_BUS on error.
 */
int8_t shtc1_id(const shtc1_t *dev, uint16_t *id);

/**
 * @brief Resets sensor
 *
 * This will reset all internal state machines and reload calibration data from the memory.
 *
 * @param[in] dev       The I2C device descriptor.
 *
 * @return              SHTC1_OK on everything done.
 * @return              SHTC1_ERROR_BUS on error.
 */
int8_t shtc1_reset(const shtc1_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* SHTC1_H */
/** @} */
