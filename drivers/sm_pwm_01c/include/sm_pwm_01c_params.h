/*
 * SPDX-FileCopyrightText: 2021 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_sm_pwm_01c
 * @{
 * @file
 * @brief       Default configuration for SM_PWM_01C driver
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 */

#include "board.h"
#include "saul_reg.h"
#include "sm_pwm_01c.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the SM_PWM_01C
 * @{
 */
#ifndef SM_PWM_01C_TSP_PIN
#define SM_PWM_01C_TSP_PIN               GPIO_PIN(0, 13)
#endif
#ifndef SM_PWM_01C_TLP_PIN
#define SM_PWM_01C_TLP_PIN               GPIO_PIN(0, 28)
#endif
#ifndef SM_PWM_01C_SAUL_INFO
#define SM_PWM_01C_SAUL_INFO             { .name = "sm-pwm-01c" }
#endif

#ifndef SM_PWM_01C_PARAMS_DEFAULT
#define SM_PWM_01C_PARAMS_DEFAULT        { .tsp_pin = SM_PWM_01C_TSP_PIN, \
                                           .tlp_pin = SM_PWM_01C_TLP_PIN }
#endif
/**@}*/

/**
 * @brief   Configure SM_PWM_01C
 */
static const sm_pwm_01c_params_t sm_pwm_01c_params[] =
{
#ifdef SM_PWM_01C_PARAMS_BOARD
    SM_PWM_01C_PARAMS_BOARD,
#else
    SM_PWM_01C_PARAMS_DEFAULT
#endif
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t sm_pwm_01c_saul_info[] =
{
    SM_PWM_01C_SAUL_INFO
};

/**
 * @brief   The number of configured sensors
 */
#define SM_PWM_01C_NUMOF    ARRAY_SIZE(sm_pwm_01c_params)

#ifdef __cplusplus
}
#endif

/** @} */
