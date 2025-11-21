/*
 * SPDX-FileCopyrightText: 2025 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    drivers_inc_encoder Generic Incremental Rotary Encoder
 * @ingroup     drivers_sensors
 * @brief       Generic incremental rotary encoder to measur RPM and rotation count.
 *
 * ## Description
 * This is a driver for a generic incremental rotary encoder. These sensors are most often
 * used with motors.
 * An example of such an encoder can be found [here](https://www.dfrobot.com/product-1617.html)
 *
 * The sensor generates a fixed number of pulses per rotation on two output pins.
 * These signals are phase-shifted by 90 degrees, enabling the detection of rotation direction.
 *
 * These encoders typically have four wires:
 *  - GNR (Ground)
 *  - VCC (Power)
 *  - Phase A
 *  - Phase B
 *
 * Phase A should be connected to the pin configured as the interrupt pin or the QDEC A pin,
 * while Phase B (the shifted signal) should be connected to the direction pin or QDEC B pin.
 * If the Phase A and Phase B connections are swapped,
 * the detected rotation direction will be reversed.
 *
 * The driver provides functions to read the current RPM
 * and the total number of revolutions since the last measurement. The number of revolutions
 * is provided in millirevolutions (thousandths of a full revolution) to allow for higher precision.
 *
 * Configuration options are available via Kconfig to specify
 * the number of pulses per rotation, the maximum RPM, and the gear reduction ratio (in tenths).
 *
 * ## Backends
 * This driver supports two backends for reading the encoder signals:
 *  - Hardware accelerated using the QDEC peripheral
 *  - Software based using GPIO interrupts
 *
 * To use either of the backends add the corresponding module to your application Makefile:
 * ```
 * USEMODULE += inc_encoder_hardware
 * ```
 * or
 * ```
 * USEMODULE += inc_encoder_software
 * ```
 *
 * ### GPIO Interrupt Backend
 * The GPIO interrupt backend uses interrupts to count the number of rising edges on Phase A and
 * determines the rotation direction based on the state of Phase B.
 * The RPM calculation is done based on the delta time between the last two rising edges.
 * If only one phase is connected, the driver will still count the pulses,
 * but there will be no direction detection.
 *
 * ### QDEC Backend
 * The QDEC backend uses the microcontroller's Quadrature Decoder peripheral to handle the counting
 * and direction detection in hardware.
 * The RPM calculation is done periodically based on the
 * pulse count provided by the QDEC peripheral.
 * If only one phase is connected, the QDEC peripheral will not count any pulses.
 * The period for RPM calculation can be configured via Kconfig.
 *
 * ## SAUL Interface
 * This driver implements the SAUL sensor interface.
 * It provides two SAUL devices:
 *  - One for reading the current RPM
 *  - One for reading the total number of revolutions since the last read.
 *
 * @note After 327 revolutions without reading and resetting
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
#include "periph/qdec.h"
#include "ztimer.h"
#include "ztimer/periodic.h"
#include "kernel_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Device initialization parameters
 */
typedef struct {
#if IS_USED(MODULE_INC_ENCODER_HARDWARE) || DOXYGEN
    qdec_t qdec_dev;  /**< QDEC device used for hardware decoding */
#elif IS_USED(MODULE_INC_ENCODER_SOFTWARE) || DOXYGEN
    gpio_t interrupt; /**< Interrupt pin (first phase) */
    gpio_t direction; /**< Pin used to determine the direction (shifted phase) */
#endif
} inc_encoder_params_t;

/**
 * @brief Device descriptor for the driver
 */
typedef struct {
    inc_encoder_params_t params;          /**< configuration parameters */
#if IS_USED(MODULE_INC_ENCODER_SOFTWARE) || DOXYGEN
    uint32_t             delta_t;         /**< time delta since the last pulse */
    int32_t              pulse_counter;   /**< number of pulses since last read */
    uint32_t             last_read_time;  /**< time of the last read */
    bool                 cw;              /**< clock wise rotation */
    bool                 stale;           /**< indicates that there is no new data to be read */
#elif IS_USED(MODULE_INC_ENCODER_HARDWARE) || DOXYGEN
    int32_t              extended_count;  /**< accumulated count of pulse overflows */
    int32_t              prev_count;      /**< number of pulses at last RPM calculation */
    int32_t              leftover_count;  /**< leftover from last reset */
    int32_t              last_rpm;        /**< last calculated RPM value */
    ztimer_periodic_t    rpm_timer;       /**< timer used to periodically calculate RPM */
#endif
} inc_encoder_t;

/**
 * @brief          Initialize the given device
 *
 * @param[in, out] dev            Device descriptor of incremental rotary encoder
 * @param[in]      params         Initialization parameters
 *
 * @retval         0              on success
 * @retval         -EIO           on failure setting up the pins
 * @retval         -EINVAL        on invalid qdec configuration
 */
int inc_encoder_init(inc_encoder_t *dev, const inc_encoder_params_t *params);

/**
 * @brief          Read the current RPM of the motor.
 *
 * @param[in]      dev            Device descriptor of incremental rotary encoder
 * @param[out]     rpm            Revolutions per minute.
 *                                Negative RPM responds to counter clock wise rotation.
 *
 * @return         0              on success
 */
int inc_encoder_read_rpm(inc_encoder_t *dev, int32_t *rpm);

/**
 * @brief          Read and reset number of revolutions since the last readout in
 *                 thousands of a full revolution.
 *
 * @param[in]      dev            Device descriptor of incremental rotary encoder
 * @param[out]     rev_counter    Number of millirevolutions since the last read.
 *                                Negative revolutions signal counter clock wise rotations.
 *
 * @return         0              on success
 */
int inc_encoder_read_reset_milli_revs(inc_encoder_t *dev, int32_t *rev_counter);

#ifdef __cplusplus
}
#endif

/** @} */
