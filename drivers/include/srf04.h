/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_srf04 srf04 ultra sonic range finder
 * @ingroup     drivers_sensors
 * @brief       Device driver for the srf04 ultra sonic range finder
 * @{
 *
 * @file
 * @brief       Device driver for the srf04 ultra sonic range finder
 *
 * @author      Semjon Kerner <semjon.kerner@fu-berlin.de>
 */

#ifndef SRF04_H
#define SRF04_H

#include <stdint.h>
#include <stdio.h>

#include "periph/gpio.h"
#include "xtimer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Status and error return codes
 */
enum {
    SRF04_OK            =  0,   /**< exit without error */
    SRF04_ERR_INVALID   = -1,   /**< error no valid measurement available*/
    SRF04_ERR_MEASURING = -2,   /**< error sensor is measuring*/
    SRF04_ERR_GPIO      = -3,   /**< error initializing gpio*/
};

/**
 * @brief   GPIO pins for srf04 device
 */
typedef struct {
    gpio_t trigger;     /**< GPIO Port the trigger pin is connected to */
    gpio_t echo;        /**< GPIO Port the echo pin is connected to */
} srf04_params_t;

/**
 * @brief   Device descriptor for srf04 sensor
 */
typedef struct {
    srf04_params_t p;   /**< GPIO Ports of device */
    int32_t distance;   /**< raw time of flight distance */
    uint32_t time;      /**< timestamp of trigger or echo */
} srf04_t;

/**
 * @brief   Initialize gpio and interrupt
 *
 * @param[out] dev      device descriptor of sensor to initialize
 * @param[in] params    init param struct holding gpio trigger and echo pins
 *
 * @return              SRF04_OK on success
 * @return              SRF04_GPIO on gpio init failure
 */
int srf04_init(srf04_t *dev, const srf04_params_t *params);

/**
 * @brief   Triggers measurement
 *
 * @param[in]  dev      device descriptor of sensor
 */
void srf04_trigger(const srf04_t *dev);

/**
 * @brief   Returns time of flight in ms
 *
 * @note    should not be invoked within 50 ms after triggering
 *
 * @param[in]  dev      device descriptor of sensor
 *
 * @return              time of flight in ms
 * @return              SRF04_MEASURING if measurement is in progress
 * @return              SRF04_INVALID if no valid measurement is available
 */
int srf04_read(const srf04_t *dev);

/**
 * @brief   Convenience function triggers a measurement and returns distance
 *
 * @note    This function will return after 50 ms once new data is available
 *
 * @param[in]  dev      device descriptor of sensor
 *
 * @return              time of flight in mm
 * @return              SRF04_MEASURING if measurement is in progress
 * @return              SRF04_INVALID if no valid measurement is available
 */
int srf04_get_distance(const srf04_t *dev);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* SRF04_H */
