/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_apa102
 * @{
 *
 * @file
 * @brief       APA102 board specific configuration
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef APA102_PARAMS_H
#define APA102_PARAMS_H

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

#define APA102_PARAMS_DEFAULT       { .led_numof = APA102_PARAM_LED_NUMOF, \
                                      .data_pin  = APA102_PARAM_DATA_PIN, \
                                      .clk_pin   = APA102_PARAM_CLK_PIN }
/**@}*/

/**
 * @brief   APA102 configuration
 */
static const  apa102_params_t apa102_params[] =
{
#ifdef APA102_PARAMS_BOARD
    APA102_PARAMS_BOARD,
#else
    APA102_PARAMS_DEFAULT,
#endif
};

#ifdef __cplusplus
}
#endif

#endif /* APA102_PARAMS_H */
/** @} */
