/*
 * Copyright (C) 2023 Prime Controls
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    drivers_soft_onewire Soft 1-Wire Bus Driver
 * @ingroup     drivers_onewire_buses
 * @ingroup     drivers_soft_periph
 * @brief       Software implemented 1-Wire bus master
 *
 * This is a soft driver implementation of a 1-Wire bus master. Minimally, the
 * only required hardware is a GPIO pin.
 *
 * The pseudomodule `soft_onewire_hwtimer` is provided. When enabled, the driver
 * will use a dedicated `periph_timer` rather than `ztimer` to manage time
 * critical bus operations.
 *
 * The pseudomodule `soft_onewire_2pins`, when enabled, separates the transmit
 * and receive functions of the bus into separate pins of the MCU. This is
 * useful to allow the TX pin to drive a transistor so that greater currents may
 * be sinked.
 *
 * If the pseudomodule `soft_onewire_pwr` is enabled, the bus will be driven
 * high (instead of pulled high via the bus's pullup resistor) whenever the bus
 * is idle. This helps supply addition current for 1-Wire devices that are
 * parasitically powered from the bus. Note that if the GPIO pin's output
 * voltage is lower than the pull up voltage, this may actually draw the bus's
 * idle voltage down to the pin's voltage, although potentially at a lower
 * output impedance.
 *
 * @{
 * @file
 * @brief       Soft 1-Wire driver interface
 *
 * @author      Joshua DeWeese <jdeweese@primecontrols.com>
 */

#include <stdbool.h>
#include <stdint.h>

#include "mutex.h"
#include "onewire.h"
#include "periph/gpio.h"
#include "periph/timer.h"
#include "ztimer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief soft_onewire_t forward declaration
 */
typedef struct soft_onewire_t soft_onewire_t;

/** timer callback function signature internally used to schedule events */
typedef void (*soft_onewire_timer_cb_t)(soft_onewire_t*);

/**
 * @brief   Soft 1-Wire configuration parameters
 */
typedef struct {
    onewire_params_t super;     /**< 1-Wire API params */
#if MODULE_SOFT_ONEWIRE_2PINS
    gpio_t tx_pin;              /**< GPIO pin for driving the bus */
    gpio_t rx_pin;              /**< GPIO pin for reading the bus */
#else
    gpio_t pin;                 /**< GPIO pin the bus is connected to */
    gpio_mode_t pin_imode;      /**< GPIO pin input mode */
#endif
#if MODULE_SOFT_ONEWIRE_HWTIMER
    tim_t timer;                /**< peripheral timer that driver should use */
#endif
} soft_onewire_params_t;

/**
 * @brief   Soft 1-Wire bus device descriptor
 */
struct soft_onewire_t {

    /** 1-Wire API instance */
    onewire_t super;

    /** mutext to sync thread with ISRs */
    mutex_t sync;

    /** Pointer to transmit and receive buffers. Only one at a time will be
        used, so they are in a union to save RAM. */
    union {
        const uint8_t *tx_buf;  /**< pointer to buffer of bits to send */
        uint8_t *rx_buf;        /**< pointer to buffer for received bits */
    };

    /** number of bits to send or receive, also used to return errors from
        ISR */
    int buf_size;

#if MODULE_SOFT_ONEWIRE_HWTIMER || DOXYGEN

    /** pointer to callback of next bus event */
    soft_onewire_timer_cb_t timer_cb;
#elif DOXYGEN
    /** timer used to trigger bus events */
    ztimer_t timer;
#endif
};

/** onewire driver callbacks for soft_onewire implementation */
extern const onewire_driver_t soft_onewire_driver;

/**
 * @brief Initialize soft 1-Wire bus
 *
 * @param[in] bus       bus descriptor
 * @param[in] params    configuration parameters
 */
void soft_onewire_init(soft_onewire_t *dev,
    const soft_onewire_params_t *params);

#ifdef __cplusplus
}
#endif

/** @} */
