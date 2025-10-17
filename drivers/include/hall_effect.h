/*
 * SPDX-FileCopyrightText: 2025 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    drivers_hall_effect Generic Hall Effect Sensor
 * @ingroup     drivers_sensors
 * @brief       Generic hall effect sensor to measur rpm and rotation count.
 *
 * ## Description
 * This is a driver for a generic Hall effect sensor.
 * The sensor produces a fixed number of pulses per rotation on two output pins.
 * The phases are slightly offset, allowing the detection of rotation direction.
 *
 * This driver provides functions to read the current RPM
 * and the total number of rotations (in hundredths) since the last measurement.
 *
 * Configuration options are available via Kconfig to specify
 * the number of pulses per rotation and the gear reduction ratio.
 * @{
 *
 * @file
 *
 * @author      Leonard Herbst <leonard.herbst@tu-dresden.de>
 */

#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Device initialization parameters
 */
typedef struct {
    gpio_t interrupt; /**< Interrupt pin */
    gpio_t direction; /**< Pin used to determine the direction */
} hall_effect_params_t;

/**
 * @brief Device descriptor for the driver
 */
typedef struct {
    hall_effect_params_t params;         /**< configuration parameters */
    uint32_t             delta_t;        /**< time delta since the last read */
    int32_t              pulse_counter;  /**< number of pulses since last read */
    uint32_t             last_read_time; /**< time of the last read */
    bool                 clock_wise;     /**< was the rotation clock wise */
    bool                 stale;          /**< if data is stale we can just return 0 rpm */
} hall_effect_t;

/**
 * @brief        Initialize the given device
 *
 * @param[inout] dev        Device descriptor of hall effect sensor
 * @param[in]    params     Initialization parameters
 *
 * @retval       0          on success
 * @retval       -EIO       on failure setting up the pins
 */
int hall_effect_init(hall_effect_t *dev, const hall_effect_params_t *params);

/**
 * @brief        Read the current rpm of the motor.
 *
 * @param[in]    dev        Device descriptor of hall effect sensor
 * @param[out]   rpm        Revolutions per minute.
 *                          Negative rpm responds to counter clock wise rotation.
 *
 * @return       0          on success
 */
int hall_effect_read_rpm(hall_effect_t *dev, int32_t *rpm);

/**
 * @brief        Read number of revolutions since the last read in hundredths.
 *
 * @param[in]    dev        Device descriptor of hall effect sensor
 * @param[out]   rpm        Number of rotation since the last read in hundredths.
 *                          Negative rotation means counter clock wise rotation.
 *
 * @return       0          on success
 */
int hall_effect_read_reset_revolutions_hundreths(hall_effect_t *dev, int32_t *pulse_counter);

#ifdef __cplusplus
}
#endif

/** @} */
