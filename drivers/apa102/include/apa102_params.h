/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_apa102
 * @{
 *
 * @file
 * @brief       APA102 board specific configuration
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */


#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Set default configuration parameters for the APA102 driver
 * @{
 */
#ifndef APA102_PARAM_LED_NUMOF
#define APA102_PARAM_LED_NUMOF      (64)    /* many have 64 per meter... */
#endif
#ifndef APA102_PARAM_DATA_PIN
#define APA102_PARAM_DATA_PIN       (GPIO_PIN(0, 0))
#endif
#ifndef APA102_PARAM_CLK_PIN
#define APA102_PARAM_CLK_PIN        (GPIO_PIN(0, 1))
#endif

#ifndef APA102_PARAMS
#define APA102_PARAMS               { .led_numof = APA102_PARAM_LED_NUMOF, \
                                      .data_pin  = APA102_PARAM_DATA_PIN, \
                                      .clk_pin   = APA102_PARAM_CLK_PIN }
#endif
/**@}*/

/**
 * @brief   APA102 configuration
 */
static const  apa102_params_t apa102_params[] =
{
    APA102_PARAMS
};

#ifdef __cplusplus
}
#endif

/** @} */
