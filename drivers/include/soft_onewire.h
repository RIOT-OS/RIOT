/*
 * Copyright (C) 2023 Prime Controls
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_soft_onewire Soft 1-Wire Bus Driver
 * @ingroup     drivers_onewire
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
 * @{
 * @file
 * @brief       1-Wire driver interface
 *
 * @author      Joshua DeWeese <jdeweese@primecontrols.com>
 */

#ifndef SOFT_ONEWIRE_H
#define SOFT_ONEWIRE_H

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
    gpio_t pin;                 /**< GPIO pin the bus is connected to */
    gpio_mode_t pin_imode;      /**< GPIO pin input mode */
#ifdef MODULE_SOFT_ONEWIRE_HWTIMER
    tim_t timer;                /**< peripheral timer that driver should use */
#endif
} soft_onewire_params_t;

/**
 * @brief   Soft 1-Wire bus device descriptor
 */
struct soft_onewire_t {
    /** devices's configuration params */
    const soft_onewire_params_t *params;

    /** mutext to sync thread with ISRs */
    mutex_t sync;

    /** TODO: rm or doc */
    uint8_t mask;

    /** Pointer to transmit and receive buffers. Only one at a time will be
        used, so they are in a union to save RAM. */
    union {
        const uint8_t *tx_buf;  /**< pointer to buffer of bits to send */
        uint8_t *rx_buf;        /**< pointer to buffer for received bits */
    };

    /** number of bits to send or receive, also used to return errors from
        ISR */
    int buf_size;

#ifdef MODULE_SOFT_ONEWIRE_HWTIMER

    /** pointer to callback of next bus event */
    soft_onewire_timer_cb_t timer_cb;
#else
    /** timer used to trigger bus events */
    ztimer_t timer;
#endif
};

/** onewire driver callbacks for soft_onewire implementation */
extern const onewire_driver_t soft_onewire_driver;

#endif /* SOFT_ONEWIRE_H */
/** @} */
