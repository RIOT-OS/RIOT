/*
 * SPDX-FileCopyrightText: 2016 Hauke Petersen <devel@haukepetersen.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_lpd8808
 *
 * @{
 * @file
 * @brief       Default configuration for LPD8808 based LED strips
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the LPD8808 driver
 * @{
 */
#ifndef LPD8808_PARAM_LED_CNT
#define LPD8808_PARAM_LED_CNT       (64)    /* most have 64 per meter... */
#endif
#ifndef LPD8808_PARAM_PIN_CLK
#define LPD8808_PARAM_PIN_CLK       (GPIO_PIN(0, 0))
#endif
#ifndef LPD8808_PARAM_PIN_DAT
#define LPD8808_PARAM_PIN_DAT       (GPIO_PIN(0, 1))
#endif

#ifndef LPD8808_PARAMS
#define LPD8808_PARAMS              { .led_cnt = LPD8808_PARAM_LED_CNT, \
                                      .pin_clk = LPD8808_PARAM_PIN_CLK, \
                                      .pin_dat = LPD8808_PARAM_PIN_DAT }
#endif
/**@}*/

/**
 * @brief   LPD8808 parameter allocation
 */
static const lpd8808_params_t lpd8808_params[] =
{
    LPD8808_PARAMS
};

#ifdef __cplusplus
}
#endif

/** @} */
