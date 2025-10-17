/*
 * SPDX-FileCopyrightText: 2025 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_hall_effect
 *
 * @{
 * @file
 * @brief       Default configuration for a generic hall effect sensor.
 *
 * @author      Leonard Herbst <leonard.herbst@tu-dresden.de>
 */

#include "board.h"
#include "hall_effect.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Default gear reduction ratio
 */
#ifndef CONFIG_HALL_EFFECT_GEAR_RED_RATIO
#  define CONFIG_HALL_EFFECT_GEAR_RED_RATIO 204
#endif

/**
 * @brief Default number of pulses per revolution
 */
#ifndef CONFIG_HALL_EFFECT_PPR
#  define CONFIG_HALL_EFFECT_PPR 13
#endif

/**
 * @name    default configuration parameters for a generic hall effect sensor
 * @{
 */
/**
 * @brief Default pin of the first phase used to trigger the interrupt
 */
#ifndef HALL_EFFECT_INTERRUPT
#  define HALL_EFFECT_INTERRUPT GPIO_PIN(1, 1)
#endif

/**
 * @brief Default pin of the second (offset) phase used to determine the direction
 */
#ifndef HALL_EFFECT_DIRECTION
#  define HALL_EFFECT_DIRECTION GPIO_PIN(1, 2)
#endif

/**
 * @brief Default parameters
 */
#ifndef HALL_EFFECT_PARAMS
#  define HALL_EFFECT_PARAMS { .interrupt = HALL_EFFECT_INTERRUPT, \
                               .direction = HALL_EFFECT_DIRECTION }
#endif
/**@}*/

/**
 * @brief SAUL info for the rpm and pulse count driver
 */
#ifndef HALL_EFFECT_SAUL_INFO
#  define HALL_EFFECT_SAUL_INFO { { .name = "Hall Effect RPM Sensor" }, \
                                  { .name = "Hall Effect Pulse Count Sensor" } }
#endif

/**
 * @brief Hall effect sensor configuraiton
 */
static const hall_effect_params_t hall_effect_params[] =
{
    HALL_EFFECT_PARAMS
};

/**
 * @brief Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t hall_effect_saul_info[][2] =
{
    HALL_EFFECT_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

/** @} */
