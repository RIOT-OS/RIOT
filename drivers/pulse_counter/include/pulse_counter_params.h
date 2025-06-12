/*
 * SPDX-FileCopyrightText: 2017 UC Berkeley
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_pulse_counter
 *
 * @{
 * @file
 * @brief       Default configuration for PULSE_COUNTER devices
 *
 * @author      Hyung-Sin Kim <hs.kim@cs.berkeley.edu>
 */

#include "board.h"
#include "pulse_counter.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    GPIO pulse counter driver initialization parameters
 * @{
 */
#ifndef PULSE_COUNTER_GPIO
/**
 * @brief   The GPIO pin to count pulses of
 */
#define PULSE_COUNTER_GPIO GPIO_PIN(0, 18)
#endif

#ifndef PULSE_COUNTER_GPIO_FLANK
/**
 * @brief   The signal flank to count pulses at
 */
#define PULSE_COUNTER_GPIO_FLANK GPIO_FALLING
#endif

#ifndef PULSE_COUNTER_PARAMS
/**
 * @brief   The pulse counter parameters to initialize the driver with
 */
#define PULSE_COUNTER_PARAMS       { .gpio = PULSE_COUNTER_GPIO, \
                                     .gpio_flank = PULSE_COUNTER_GPIO_FLANK }
#endif

#ifndef PULSE_COUNTER_SAUL_INFO
/**
 * @brief   The SAUL info to use when registering the pulse counter instances
 */
#define PULSE_COUNTER_SAUL_INFO    { .name = "pulse counter" }
#endif

/**
 * @brief   PULSE_COUNTER configuration
 */
static const pulse_counter_params_t pulse_counter_params[] =
{
    PULSE_COUNTER_PARAMS,
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t pulse_counter_saul_info[] =
{
    PULSE_COUNTER_SAUL_INFO
};

/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
