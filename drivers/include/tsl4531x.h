/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_tsl4531x TSL4531x Illuminance sensor
 * @ingroup     drivers_sensors
 * @brief       Device driver for the AMS TSL4531 sensor
 * @{
 *
 * @file
 * @brief       Device driver for the AMS TSL4531 sensor
 *
 * @author      Juan I Carrano <j.carrano@fu-berlin.de>
 */

#ifndef TSL4531x_H
#define TSL4531x_H

#include <stdint.h>

#include "periph/i2c.h"

static const uint16_t TSL4531x_INTEGRATE_400ms = 0; /* 0b00 */
static const uint16_t TSL4531x_INTEGRATE_200ms = 1; /* 0b01 */
static const uint16_t TSL4531x_INTEGRATE_100ms = 2; /* 0b10 */

/**
 * @brief   Represents a single TSL4531x device.
 */
typedef struct tsl4531x {
    i2c_t i2c_dev;
    uint8_t integration_time;
} tsl4531x_t;

/**
 * Initialize TSL4513 device.
 *
 * @param   dev     Device object.
 * @param   i2c_dev I2C interface to use.
 * @param   integration_time  one of TSL4531x_INTEGRATE_400ms,
 *                              TSL4531x_INTEGRATE_200ms, TSL4531x_INTEGRATE_100ms.
 *
 * @return  Zero on success, negative on error.
 */
int tsl4531x_init(tsl4531x_t *dev, i2c_t i2c_dev, uint8_t integration_time);

/**
 * Get the last measurements of illuminance, in lux.
 *
 * @return  0 on sucess, negative on error. If there is an error, the value of
 *           `*result_lux` is undefined.
 */
int tsl4531x_read(const tsl4531x_t *dev, uint16_t *result_lux);

#endif /* TSL4531x_H */
