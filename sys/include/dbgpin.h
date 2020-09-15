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

extern const gpio_t dbgpin_pins[];

static inline void dbgpin_set(unsigned pin)
{
    gpio_set(dbgpin_pins[pin]);
}

static inline void dbgpin_clr(unsigned pin)
{
    gpio_clear(dbgpin_pins[pin]);
}

static inline void dbgpin_tgl(unsigned pin)
{
    gpio_toggle(dbgpin_pins[pin]);
}

static inline void dbgpin_pulse(unsigned pin)
{
    dbgpin_tgl(pin);
    dbgpin_tgl(pin);
}

static inline void dbgpin_sig(unsigned pin, unsigned num)
{
    for (unsigned i = 0; i < num; i++) {
        dbgpin_pulse(pin);
    }
}

unsigned dbgpin_cnt(void);

void dbgpin_init(void);

#ifdef __cplusplus
}
#endif

#endif /* DBGPIN_H */
/** @} **/
