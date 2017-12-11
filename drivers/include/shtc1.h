/*
 * Copyright 2017, RWTH Aachen. All rights reserved.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_shtc1
 * @ingroup     drivers_sensors
 * @name        Device driver interface for the SHTC1 Temperature and humidity sensor
 * @{
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
#include "periph/i2c.h"

#ifdef __cplusplus 
extern "C" {
#endif

typedef enum {
    CRC_DISABLED = 0,
    CRC_ENABLED
} shtc1_crc_type_t;

typedef struct {
    float temp;
    float rel_humidity;
    unsigned int id;
} shtc1_values_t;

typedef struct {
    i2c_t bus;
    uint8_t addr;
    shtc1_crc_type_t crc;
} shtc1_params_t;

typedef struct {
    i2c_t bus;
    uint8_t addr;
    shtc1_values_t values;
    shtc1_crc_type_t crc;
} shtc1_t;

enum {
    SHTC1_OK = 0,
    SHTC1_ERROR = -1
};

/**
 * @brief Initializes the sensor and I2C
 *
 * @param[in] dev       I2C device descriptor
 * @param[in] params    SHTC1 parameters to be used
 *
 * @return              SHTC1_OK on a working initialization
 * @return              SHTC1_ERROR on error
 */
int8_t shtc1_init(shtc1_t* const dev, const shtc1_params_t* params);

/**
 * @brief Reads temperature and humidity values
 * @details The values wil be saved in the device descriptor (values struct). The temperature is in °C and the humidity in %
 *
 * @param[in] dev       The I2C device descriptor
 *
 * @return              SHTC1_OK if a measurement completed. The values will be stored in the values struct. Temperature in °C and humidity in %
 * @return              SHTC1_ERROR on checksum error
 */
int8_t shtc1_measure(shtc1_t* const dev);

/**
 * @brief Reads the ID and saves it in the device descriptor
 *
 * @details             When working correctly ID should equal xxxx'xxxx'xx00'0111 where x is unspecified
 *
 * @param[in] dev       The I2C device descriptor
 *
 * @return              SHTC1_OK on everything done
 * @return              SHTC1_ERROR on error
 */
int8_t shtc1_id(shtc1_t* const dev);

/**
 * @brief Resets sensor
 *
 * This will reset all internal state machines and reload calibration data from the memory
 *
 * @param[in] dev       The I2C device descriptor
 *
 * @return              SHTC1_OK on everything done
 * @return              SHTC1_ERROR on error
 */
int8_t shtc1_reset(const shtc1_t* const dev);

#ifdef __cplusplus
}
#endif

#endif /*SHTC1_H*/
/**@}*/
