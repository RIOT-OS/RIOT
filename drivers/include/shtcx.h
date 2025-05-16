/*
 * Copyright 2017, RWTH Aachen. All rights reserved.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    drivers_shtcx SHTCX Temperature and humidity sensor
 * @ingroup     drivers_sensors
 * @ingroup     drivers_saul
 * @brief       Driver for the Sensirion SHTCX sensor.
 *
 * @{
 *
 * @file
 * @brief       Device driver interface for the SHTCX Temperature and humidity sensor
 *
 * @author      Steffen Robertz <steffen.robertz@rwth-aachen.de>
 * @author      Josua Arndt <jarndt@ias.rwth-aachen.de>
 */

#include <stdint.h>
#include "saul.h"

#include "periph/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   SHTCX Default Address
 *
 */
#ifndef SHTCX_I2C_ADDRESS
#define SHTCX_I2C_ADDRESS         (0x70)
#endif

/**
 * @brief   settings struct with all relevant parameters
 * @{
 */
typedef struct {
    i2c_t i2c_dev;              /**< I2C bus descriptor. */
    uint8_t i2c_addr;           /**< I2C address of the sensor. */
} shtcx_params_t;
/** @} */

/**
 * @brief   device descriptor for the SHTCX
 * @{
 */
typedef struct {
    shtcx_params_t params;  /**< Parameters struct with all settings set. */
} shtcx_t;
/** @} */

enum {
    SHTCX_OK,           /**< Success, no error */
    SHTCX_ERROR_BUS,    /**< I2C bus error */
    SHTCX_ERROR_CRC,    /**< CRC error */
    SHTCX_ERROR         /**< General error */
};

/**
 * @brief Initializes the sensor and I2C.
 *
 * @param[in] dev       I2C device descriptor.
 * @param[in] params    SHTCX parameters to be used.
 *
 * @return              SHTCX_OK on a working initialization.
 * @return              SHTCX_ERROR_BUS Reading I2C failed.
 * @return              SHTCX_ERROR_CRC Wrong ID.
 */
int8_t shtcx_init(shtcx_t* const dev, const shtcx_params_t* params);

/**
 * @brief Reads all register values from the device.
 * @details The values as raw data will be saved into received.
 *
 * @param[in] dev             The I2C device descriptor.
 * @param[in] rel_humidity    Humidity in centi %.
 * @param[in] temperature     Temperature in centi °C.
 *
 * @return              SHTCX_OK if a measurement completed.
 * @return              SHTCX_ERROR_BUS reading I2C failed.
 * @return              SHTCX_ERROR_CRC on checksum error.
 */
int8_t shtcx_read(const shtcx_t *dev, uint16_t *rel_humidity, int16_t *temperature);

/**
 * @brief Reads the ID and saves it in the device descriptor
 *
 * @details             When working correctly ID should equal
 *                      xxxx'xxxx'xx00'0111 where x is unspecified.
 *
 * @param[in] dev       The I2C device descriptor.
 * @param[in] id        ID of the device.
 *
 * @return              SHTCX_OK on everything done.
 * @return              SHTCX_ERROR_BUS on error.
 */
int8_t shtcx_id(const shtcx_t *dev, uint16_t *id);

/**
 * @brief Resets sensor
 *
 * This will reset all internal state machines and reload calibration data from the memory.
 *
 * @param[in] dev       The I2C device descriptor.
 *
 * @return              SHTCX_OK on everything done.
 * @return              SHTCX_ERROR_BUS on error.
 */
int8_t shtcx_reset(const shtcx_t *dev);

#ifdef __cplusplus
}
#endif

/** @} */
