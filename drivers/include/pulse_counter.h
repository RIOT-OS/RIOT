/*
 * Copyright (C) 2017 UC Berkeley
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_pulse_counter extra sensor
 * @ingroup     drivers_sensors
 *
 * The connection between the MCU and the PULSE_COUNTER is based on the
 * GPIO-interface.
 *
 * @{
 *
 * @file
 * @brief       Driver for the PULSE_COUNTER extra sensor
 *
 * @author      Hyung-Sin Kim <hs.kim@cs.berkeley.edu>
 */

#ifndef PULSE_COUNTER_H
#define PULSE_COUNTER_H

#include <stdint.h>
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Parameters needed for device initialization
 */
typedef struct {
    gpio_t       gpio;         /**< GPIO pin that sensor is connected to */
    gpio_flank_t gpio_flank;   /**< GPIO flank option */
} pulse_counter_params_t;

/**
  * @brief   Device descriptor for a PULSE_COUNTER device
  */
typedef struct {
    int16_t pulse_count;       /**< pulse counter */
} pulse_counter_t;

/**
 * @brief   Initialize an PULSE_COUNTER device
 *
 * @param[out] dev          device descriptor
 * @param[in] params        configuration parameters
 *
 * @return                   0 on success
 * @return                  -1 on error
 */
int pulse_counter_init(pulse_counter_t *dev, const pulse_counter_params_t *params);

/**
 * @brief   Read and reset PULSE_COUNTER value
 *
 * @param[in]  dev          device descriptor of sensor
 *
 * @return                  Accumulated pulse counts
 */
int16_t pulse_counter_read_with_reset(const void *dev);

/**
 * @brief   Read PULSE_COUNTER value
 *
 * @param[in]  dev          device descriptor of sensor
 *
 * @return                  Accumulated pulse counts
 */
int16_t pulse_counter_read_without_reset(const void *dev);

/**
 * @brief   Reset PULSE_COUNTER value
 *
 * @param[in]  dev          device descriptor of sensor
 */
void pulse_counter_reset(const void *dev);

#ifdef __cplusplus
}
#endif

#endif /* PULSE_COUNTER_H */
/** @} */
