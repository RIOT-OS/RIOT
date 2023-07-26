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

/** default 1-wire bus pin */
#ifndef SOFT_ONEWIRE_PARAMS_PIN
#  define SOFT_ONEWIRE_PARAMS_PIN GPIO_UNDEF
#endif

/** default input mode for bus pin */
#ifndef SOFT_ONEWIRE_PARAMS_PIN_IMODE
#  define SOFT_ONEWIRE_PARAMS_PIN_IMODE GPIO_IN_PU
#endif

/** default 1-wire bus TX pin */
#ifndef SOFT_ONEWIRE_PARAMS_TX_PIN
#  define SOFT_ONEWIRE_PARAMS_TX_PIN GPIO_UNDEF
#endif

/** default 1-wire bus RX pin */
#ifndef SOFT_ONEWIRE_PARAMS_RX_PIN
#  define SOFT_ONEWIRE_PARAMS_RX_PIN GPIO_UNDEF
#endif

/** default timer to use for the bus */
#ifndef SOFT_ONEWIRE_PARAMS_HWTIMER
#  define SOFT_ONEWIRE_PARAMS_HWTIMER TIMER_UNDEF
#endif

/**
 * @brief Soft 1-Wire default configuration parameters
 */
static soft_onewire_params_t soft_onewire_params[] = {
    {
#if MODULE_ONEWIRE_MULTIDRIVER
        .super = {
            .driver = &soft_onewire_driver,
        },
#endif

#if MODULE_SOFT_ONEWIRE_2PINS
        .tx_pin = SOFT_ONEWIRE_PARAMS_TX_PIN,
        .rx_pin = SOFT_ONEWIRE_PARAMS_RX_PIN,
#else
        .pin = SOFT_ONEWIRE_PARAMS_PIN,
        .pin_imode = SOFT_ONEWIRE_PARAMS_PIN_IMODE,
#endif

#if MODULE_SOFT_ONEWIRE_HWTIMER
        .timer = SOFT_ONEWIRE_PARAMS_HWTIMER,
#endif
    },
};

/** number of soft 1-wire buses */
#define SOFT_ONEWIRE_NUMOF ARRAY_SIZE(soft_onewire_params)

#ifdef __cplusplus
}
#endif

/** @} */
