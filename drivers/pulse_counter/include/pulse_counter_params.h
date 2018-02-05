/*
 * Copyright (C) 2017 UC Berkeley
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_pulse_counter
 *
 * @{
 * @file
 * @brief       Default configuration for PULSE_COUNTER devices
 *
 * @author      Hyung-Sin Kim <hs.kim@cs.berkeley.edu>
 */

#ifndef PULSE_COUNTER_PARAMS_H
#define PULSE_COUNTER_PARAMS_H

#include "board.h"
#include "pulse_counter.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef PULSE_COUNTER_GPIO
#define PULSE_COUNTER_GPIO GPIO_PIN(0,18)
#endif

#ifndef PULSE_COUNTER_GPIO_FLANK
#define PULSE_COUNTER_GPIO_FLANK GPIO_FALLING
#endif

#ifndef PULSE_COUNTER_PARAMS
#define PULSE_COUNTER_PARAMS       { .gpio = PULSE_COUNTER_GPIO, \
                                     .gpio_flank = PULSE_COUNTER_GPIO_FLANK }
#endif

#ifndef PULSE_COUNTER_SAUL_INFO
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

#ifdef __cplusplus
}
#endif

#endif /* PULSE_COUNTER_PARAMS_H */
/** @} */
