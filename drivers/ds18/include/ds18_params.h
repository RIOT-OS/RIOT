/*
 * SPDX-FileCopyrightText: 2017 Frits Kuipers
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_ds18
 *
 * @{
 * @file
 * @brief       Default configuration for DS1822 and DS18B20 temperature sensors
 *
 * @author      Frits Kuipers <frits.kuipers@gmail.com>
 */

#include "board.h"
#include "ds18.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Set default configuration parameters for the ds18
 * @{
 */
#ifndef DS18_PARAM_PIN
#define DS18_PARAM_PIN             (GPIO_PIN(0, 0))
#endif
#ifndef DS18_PARAM_PULL
#define DS18_PARAM_PULL            (GPIO_OD_PU)
#endif

#define DS18_PARAMS_DEFAULT        { .pin     = DS18_PARAM_PIN, \
                                     .out_mode = DS18_PARAM_PULL }
/**@}*/

/**
 * @brief   Configure ds18
 */
static const ds18_params_t ds18_params[] =
{
#ifdef DS18_PARAMS_BOARD
    DS18_PARAMS_BOARD,
#else
    DS18_PARAMS_DEFAULT,
#endif
};

/**
 * @brief   Configure SAUL registry entries
 */
static const saul_reg_info_t ds18_saul_reg_info[] =
{
    { .name = "ds18" }
};

#ifdef __cplusplus
}
#endif

/** @} */
