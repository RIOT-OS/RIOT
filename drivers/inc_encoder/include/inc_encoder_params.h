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
 * @brief Default number of pulses per revolution
 */
#if IS_USED(MODULE_INC_ENCODER_HARDWARE)
#  ifndef CONFIG_INC_ENCODER_HARDWARE_PERIOD_MS
#    define CONFIG_INC_ENCODER_HARDWARE_PERIOD_MS 200
#  endif
#endif

/**
 * @name    default configuration parameters for a generic incremental rotary encoder
 * @{
 */
#if IS_USED(MODULE_INC_ENCODER_SOFTWARE)
/**
 * @brief Default pin of the first phase used to trigger the interrupt for software decoding
 */
#  ifndef INC_ENCODER_INTERRUPT
#    define INC_ENCODER_INTERRUPT GPIO_PIN(1, 10)
#  endif

/**
 * @brief Default pin of the second (shifted) phase used to determine
 *        the direction for software decoding
 */
#  ifndef INC_ENCODER_DIRECTION
#    define INC_ENCODER_DIRECTION GPIO_PIN(1, 11)
#  endif
#endif /* IS_USED(MODULE_INC_ENCODER_SOFTWARE) */

#if IS_USED(MODULE_INC_ENCODER_HARDWARE)
/**
 * @brief Default QDEC device used for hardware decoding
 */
#ifndef INC_ENCODER_QDEC_DEV
#  define INC_ENCODER_QDEC_DEV QDEC_DEV(0)
#endif
#endif /* IS_USED(MODULE_INC_ENCODER_HARDWARE) */

/**
 * @brief Default parameters
 */
#if IS_USED(MODULE_INC_ENCODER_SOFTWARE)
#  ifndef INC_ENCODER_PARAMS
#    define INC_ENCODER_PARAMS { .interrupt = INC_ENCODER_INTERRUPT, \
                                 .direction = INC_ENCODER_DIRECTION }
#  endif
#elif IS_USED(MODULE_INC_ENCODER_HARDWARE)

#  ifndef INC_ENCODER_PARAMS
#    define INC_ENCODER_PARAMS { .qdec_dev = INC_ENCODER_QDEC_DEV }
#  endif
#endif
/**@}*/

/**
 * @brief SAUL info for the RPM and pulse count driver
 */
#ifndef INC_ENCODER_SAUL_INFO
#  define INC_ENCODER_SAUL_INFO { { .name = "Incremental Rotary Encoder RPM Sensor" }, \
                                  { .name = "Incremental Rotary Encoder Revolution Counter" } }
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
