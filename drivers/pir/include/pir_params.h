/*
 * SPDX-FileCopyrightText: 2018 UC Berkeley
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_pir
 *
 * @{
 * @file
 * @brief       Default configuration for PIR devices
 *
 * @author      Hyung-Sin Kim <hs.kim@cs.berkeley.edu>
 */

#include "board.h"
#include "pir.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    default configuration parameters for the PIR driver
 * @{
 */
#ifndef PIR_PARAM_GPIO
/**
 * @brief   GPIO pin the PIR is connected to
 */
#define PIR_PARAM_GPIO             GPIO_PIN(0, 6)
#endif
#ifndef PIR_PARAM_ACTIVE_HIGH
/**
 * @brief   Signal polarity: 1 means high on active, 0 means low on active
 */
#define PIR_PARAM_ACTIVE_HIGH      1
#endif

#ifndef PIR_PARAMS
/**
 * @brief   The PIR configuration parameters
 */
#define PIR_PARAMS                 { .gpio = PIR_PARAM_GPIO, \
                                     .active_high = PIR_PARAM_ACTIVE_HIGH }
#endif

#ifndef PIR_SAUL_INFO
/**
 * @brief   The SAUL information the PIR devices get registered with
 */
#define PIR_SAUL_INFO              { .name = "PIR" }
#endif
/**@}*/

/**
 * @brief   PIR configuration
 */
static const pir_params_t pir_params[] =
{
    PIR_PARAMS
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t pir_saul_info[] =
{
    PIR_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

/** @} */
