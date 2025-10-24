/*
 * SPDX-FileCopyrightText: 2025 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_inc_encoder
 *
 * @{
 * @file
 * @brief       Default configuration for a generic incremental rotary encoder.
 *
 * @author      Leonard Herbst <leonard.herbst@tu-dresden.de>
 */

#include "board.h"
#include "inc_encoder.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Default gear reduction ratio
 */
#ifndef CONFIG_INC_ENCODER_GEAR_RED_RATIO
#  define CONFIG_INC_ENCODER_GEAR_RED_RATIO 204
#endif

/**
 * @brief Default number of pulses per revolution
 */
#ifndef CONFIG_INC_ENCODER_PPR
#  define CONFIG_INC_ENCODER_PPR 13
#endif

/**
 * @name    default configuration parameters for a generic incremental rotary encoder
 * @{
 */
/**
 * @brief Default pin of the first phase used to trigger the interrupt
 */
#ifndef INC_ENCODER_INTERRUPT
#  define INC_ENCODER_INTERRUPT GPIO_PIN(1, 1)
#endif

/**
 * @brief Default pin of the second (shifted) phase used to determine the direction
 */
#ifndef INC_ENCODER_DIRECTION
#  define INC_ENCODER_DIRECTION GPIO_PIN(1, 2)
#endif

/**
 * @brief Default parameters
 */
#ifndef INC_ENCODER_PARAMS
#  define INC_ENCODER_PARAMS { .interrupt = INC_ENCODER_INTERRUPT, \
                               .direction = INC_ENCODER_DIRECTION }
#endif
/**@}*/

/**
 * @brief SAUL info for the RPM and pulse count driver
 */
#ifndef INC_ENCODER_SAUL_INFO
#  define INC_ENCODER_SAUL_INFO { { .name = "Incremental Rotary Encoder RPM Sensor" }, \
                                  { .name = "Incremental Rotary Encoder Pulse Count Sensor" } }
#endif

/**
 * @brief Incremental rotary encoder configuration
 */
static const inc_encoder_params_t inc_encoder_params[] =
{
    INC_ENCODER_PARAMS
};

/**
 * @brief Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t inc_encoder_saul_info[][2] =
{
    INC_ENCODER_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

/** @} */
