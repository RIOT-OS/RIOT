/*
 * SPDX-FileCopyrightText: 2025 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    drivers_hall_effect Generic Hall Effect Sensor
 * @ingroup     drivers_sensors
 * @brief       Generic hall effect sensor to measur RPM and rotation count.
 *
 * ## Description
 * This is a driver for a generic Hall effect sensor. These sensors are most often
 * used with motors.
 * The sensor generates a fixed number of pulses per rotation on two output pins.
 * These signals are phase-shifted slightly, enabling the detection of rotation direction.
 *
 * These encoders typically have four wires:
 *  - GNR (Ground)
 *  - VCC (Power)
 *  - Phase A
 *  - Phase B
 *
 * Phase A should be connected to the pin configured as the interrupt pin,
 * while Phase B (the shifted signal) should be connected to the direction pin.
 *
 * If the Phase A and Phase B connections are swapped, the detected rotation direction will be reversed.
 * When only Phase A is connected, the encoder will still report movement,
 * but there will be no distinction between counter clock wise and clock wise rotation.
 * An example for such an encoder can be found [here](https://www.dfrobot.com/product-1617.html)
 *
 * The driver provides functions to read the current RPM
 * and the total number of revolutions (in hundredths) since the last measurement.
 *
 * Configuration options are available via Kconfig to specify
 * the number of pulses per rotation and the gear reduction ratio (in tenths).
 *
 * @note After approximately 327 revolutions without reading and resetting
 *       the revolution counter, the `phydat` value will overflow.
 *       Use the regular driver interface instead of SAUL if necessary.
 *
 * @{
 *
 * @file
 *
 * @author      Leonard Herbst <leonard.herbst@tu-dresden.de>
 */

#include <stdint.h>

#include "periph/gpio.h"
#include "irq.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Device initialization parameters
 */
typedef struct {
    gpio_t interrupt; /**< Interrupt pin (first phase) */
    gpio_t direction; /**< Pin used to determine the direction (shifted phase) */
} hall_effect_params_t;

/**
 * @brief Device descriptor for the driver
 */
typedef struct {
    hall_effect_params_t params;          /**< configuration parameters */
    uint32_t             delta_t;         /**< time delta since the last read */
    int32_t              pulse_counter;   /**< number of pulses since last read */
    uint32_t             last_read_time;  /**< time of the last read */
    bool                 ccw;             /**< counter clock wise rotation */
    bool                 stale;           /**< indicates that there is no new data to be read */
} hall_effect_t;

/**
 * @brief        Initialize the given device
 *
 * @param[inout] dev            Device descriptor of hall effect sensor
 * @param[in]    params         Initialization parameters
 *
 * @retval       0              on success
 * @retval       -EIO           on failure setting up the pins
 */
int hall_effect_init(hall_effect_t *dev, const hall_effect_params_t *params);

/**
 * @brief        Read the current RPM of the motor.
 *
 * @param[in]    dev            Device descriptor of hall effect sensor
 * @param[out]   rpm            Revolutions per minute.
 *                              Negative RPM responds to counter clock wise rotation.
 *
 * @return       0              on success
 */
int hall_effect_read_rpm(hall_effect_t *dev, int32_t *rpm);

/**
 * @brief        Read and reset number of revolutions since the last readout in hundredths.
 *
 * @param[in]    dev            Device descriptor of hall effect sensor
 * @param[out]   pulse_counter  Number of revolutions since the last read in hundredths.
 *                              Negative revolutions signal counter clock wise rotations.
 *
 * @return       0              on success
 */
int hall_effect_read_reset_ceti_revs(hall_effect_t *dev, int32_t *pulse_counter);

#ifdef __cplusplus
}
#endif

/** @} */
