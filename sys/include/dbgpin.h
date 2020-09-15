/*
 * Copyright (C) 2020 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_dbgpin Direct pin control for debugging/profiling
 * @ingroup     sys
 *
 * @warning     This module does not verify the given pin number, so make sure
 *              the pin numbers you use are actually configured!
 *
 * @{
 * @file
 * @brief       GPIO wrapper for debugging/profiling purposes
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef DBGPIN_H
#define DBGPIN_H

#include "kernel_defines.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef DBGPIN_PINS
#error Please specify the pins to use with the dbgpin module (DBGPIN_PINS)
#endif

// extern const gpio_t dbgpin_pins[];

/**
 * @brief   Set the given debug pin to HIGH
 *
 * @param[in] pin       pin to set, pin number is offset to the list of defined
 *                      debug pins in DBGPIN_PINS
 */
static inline void dbgpin_set(unsigned pin)
{
    static const gpio_t dbgpin_pins[] = { DBGPIN_PINS };
    gpio_set(dbgpin_pins[pin]);
}

/**
 * @brief   Set the given debug pin to LOW
 *
 * @param[in] pin       pin to set, pin number is offset to the list of defined
 *                      debug pins in DBGPIN_PINS
 */
static inline void dbgpin_clear(unsigned pin)
{
    static const gpio_t dbgpin_pins[] = { DBGPIN_PINS };
    gpio_clear(dbgpin_pins[pin]);
}

/**
 * @brief   Toggle the given debug pin
 *
 * @param[in] pin       pin to set, pin number is offset to the list of defined
 *                      debug pins in DBGPIN_PINS
 */
static inline void dbgpin_toggle(unsigned pin)
{
    static const gpio_t dbgpin_pins[] = { DBGPIN_PINS };
    gpio_toggle(dbgpin_pins[pin]);
}

/**
 * @brief   Output a pulse on the given debug pin (toggles the pin twice)
 *
 * @param[in] pin       pin to set, pin number is offset to the list of defined
 *                      debug pins in DBGPIN_PINS
 */
static inline void dbgpin_pulse(unsigned pin)
{
    dbgpin_toggle(pin);
    dbgpin_toggle(pin);
}

/**
 * @brief   Output a specified number of pulses on the given debug pin
 *
 * @param[in] pin       pin to set, pin number is offset to the list of defined
 *                      debug pins in DBGPIN_PINS
 * @param[in] num       number of pulses to output
 */
static inline void dbgpin_signal(unsigned pin, unsigned num)
{
    for (unsigned i = 0; i < num; i++) {
        dbgpin_pulse(pin);
    }
}

/**
 * @brief   Get the number of configured debug pins
 *
 * @return  number of configured debug pins
 */
static inline size_t dbgpin_count(void)
{
    static const gpio_t dbgpin_pins[] = { DBGPIN_PINS };
    return ARRAY_SIZE(dbgpin_pins);
}


/**
 * @brief   Initialize the configured input pins
 */
static inline void dbgpin_init(void)
{
    static const gpio_t dbgpin_pins[] = { DBGPIN_PINS };
    for (unsigned i = 0; i < ARRAY_SIZE(dbgpin_pins); i++) {
        gpio_init(dbgpin_pins[i], GPIO_OUT);
        gpio_clear(dbgpin_pins[i]);
    }
}

#ifdef __cplusplus
}
#endif

#endif /* DBGPIN_H */
/** @} **/
