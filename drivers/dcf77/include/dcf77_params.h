/*
 * SPDX-FileCopyrightText: 2019 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_dcf77
 *
 * @{
 * @file
 * @brief       Default configuration for DCF77 devices
 *
 * @author      Michel Gerlach <michel.gerlach@haw-hamburg.de>
 */

#include "board.h"
#include "dcf77.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the DCF77 devices
 * @{
 */
#ifndef DCF77_PARAM_PIN
#define DCF77_PARAM_PIN               (GPIO_PIN(1, 22))
#endif
#ifndef DCF77_PARAM_PULL
#define DCF77_PARAM_PULL              (GPIO_IN)
#endif
#ifndef DCF77_PARAMS
#define DCF77_PARAMS                  { .pin     = DCF77_PARAM_PIN, \
                                        .in_mode = DCF77_PARAM_PULL }
#endif
/**@}*/

/**
 * @brief   Configure DCF77 devices
 */
static const dcf77_params_t dcf77_params[] =
{
    DCF77_PARAMS
};

#ifdef __cplusplus
}
#endif

/** @} */
