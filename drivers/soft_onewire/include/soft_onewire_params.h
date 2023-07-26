/*
 * Copyright (C) 2023 Prime Controls
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_soft_onewire
 *
 * @{
 * @file
 * @brief       Default configuration for soft 1-Wire driver
 *
 * @author      Joshua DeWeese <jdeweese@primecontrols.com>
 */

#include "board.h"

#include "container.h"
#include "soft_onewire.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef SOFT_ONEWIRE_PARAMS_PIN
#define SOFT_ONEWIRE_PARAMS_PIN GPIO_UNDEF
#endif

#ifndef SOFT_ONEWIRE_PARAMS_PIN_IMODE
#define SOFT_ONEWIRE_PARAMS_PIN_IMODE GPIO_IN_PU
#endif

#ifndef SOFT_ONEWIRE_PARAMS_HWTIMER
#define SOFT_ONEWIRE_PARAMS_HWTIMER TIMER_UNDEF
#endif

#ifdef MODULE_SOFT_ONEWIRE_HWTIMER
#define SOFT_ONEWIRE_PARAMS_HWTIMER_ .timer = SOFT_ONEWIRE_PARAMS_HWTIMER,
#else
#define SOFT_ONEWIRE_PARAMS_HWTIMER_
#endif

#ifndef SOFT_ONEWIRE_PARAMS
#define SOFT_ONEWIRE_PARAMS \
    { \
        .pin = SOFT_ONEWIRE_PARAMS_PIN, \
        .pin_imode = SOFT_ONEWIRE_PARAMS_PIN_IMODE, \
        SOFT_ONEWIRE_PARAMS_HWTIMER_ \
    }
#endif

static const soft_onewire_params_t soft_onewire_params[] = {
    SOFT_ONEWIRE_PARAMS
};

#define SOFT_ONEWIRE_NUMOF ARRAY_SIZE(soft_onewire_params)

#ifdef __cplusplus
}
#endif

/** @} */
