/*
 * Copyright (C) 2017 UC Berkeley
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_pulse_counter Pulse counter
 * @ingroup     drivers_sensors
 * @ingroup     drivers_saul
 *
 * @brief       GPIO based pulse counting driver
 *
 * This driver provides @ref drivers_saul capabilities.
 * @{
 *
 * @file
 * @brief       Driver for the pulse counter
 *
 * @author      Hyung-Sin Kim <hs.kim@cs.berkeley.edu>
 */

#ifndef PULSE_COUNTER_H
#define PULSE_COUNTER_H

#include <stdint.h>
#ifdef __cplusplus
#include "c11_atomics_compat.hpp"
#else
#include <stdatomic.h>
#endif
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
  * @brief   Device descriptor for a pulse counter device
  */
typedef struct {
    atomic_uint_least16_t pulse_count;  /**< pulse counter */
} pulse_counter_t;

/**
 * @brief   Initialize a pulse counter device
 *
 * @param[out] dev          device descriptor
 * @param[in] params        configuration parameters
 *
 * @return                   0 on success
 * @return                  -1 on error
 */
int pulse_counter_init(pulse_counter_t *dev, const pulse_counter_params_t *params);

/**
 * @brief   Read and reset pulse counter value
 *
 * @param[out] dev          device descriptor of sensor
 *
 * @return                  Accumulated pulse counts
 */
int16_t pulse_counter_read_with_reset(pulse_counter_t *dev);

/**
 * @brief   Read pulse counter value
 *
 * @param[in]  dev          device descriptor of sensor
 *
 * @return                  Accumulated pulse counts
 */
int16_t pulse_counter_read_without_reset(pulse_counter_t *dev);

/**
 * @brief   Reset pulse counter value
 *
 * @param[out] dev         device descriptor of sensor
 */
void pulse_counter_reset(pulse_counter_t *dev);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* PULSE_COUNTER_H */
