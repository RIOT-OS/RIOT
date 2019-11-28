/*
 * Copyright (C) 2018 UC Berkeley
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_apds9007 APDS9007 light sensor
 * @ingroup     drivers_sensors
 * @brief       Driver for the APDS9007 light sensor
 *
 * The connection between the MCU and the APDS9007 is based on the
 * GPIO-interface.
 *
 * @{
 *
 * @file
 * @brief       Driver for the APDS9007 light sensor
 *
 * @author      Hyung-Sin Kim <hs.kim@cs.berkeley.edu>
 */

#ifndef APDS9007_H
#define APDS9007_H

#include <stdint.h>
#include "periph/gpio.h"
#include "periph/adc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Stabilization time for APDS9007 output
 */
#ifndef APDS9007_STABILIZATION_TIME
#define APDS9007_STABILIZATION_TIME 20000UL
#endif

/**
 * @brief   APDS9007 specific return values
 */
enum {
    APDS9007_OK      = 0,    /**< everything went as expected */
    APDS9007_ADCERR = -1,    /**< ADC reading error */
};

/**
 * @brief   Parameters needed for device initialization
 */
typedef struct {
    gpio_t    gpio;            /**< GPIO pin that sensor is connected to */
    adc_t     adcline;         /**< ADC channel that sensor is connected to */
    adc_res_t adcres;          /**< ADC resolution that sensor uses */
} apds9007_params_t;

/**
  * @brief   Device descriptor for a APDS9007 device
  * @{
  */
typedef struct {
    apds9007_params_t p;   /**< device driver configuration */
} apds9007_t;
/** @} */

/**
 * @brief   Initialize an APDS9007 device
 *
 * @param[out] dev          device descriptor
 * @param[in] params        parameters for device
 */
void apds9007_init(apds9007_t* dev, const apds9007_params_t* params);

/**
 * @brief   Activate APDS9007 device
 *
 * @param[in] dev          device descriptor
 */
void apds9007_set_active(const apds9007_t* dev);

/**
 * @brief   Turn off APDS9007 device
 *
 * @param[in] dev          device descriptor
 */
void apds9007_set_idle(const apds9007_t* dev);

/**
 * @brief   Read illuminance value
 *
 * @param[in]  dev          device descriptor
 * @param[out] light        Illuminance in raw voltage
 *
 * @return                  APDS9007_OK on success
 * @return                  APDS9007_ADCERR on ADC error
 */
int apds9007_read(const apds9007_t* dev, int16_t *light);
#ifdef __cplusplus
}
#endif

/** @} */
#endif /* APDS9007_H */
