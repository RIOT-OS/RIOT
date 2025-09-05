/*
 * SPDX-FileCopyrightText: 2017 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_dsp0401
 *
 * @{
 * @file
 * @brief       Default configuration for DSP0401
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#include "board.h"
#include "dsp0401.h"
#include "periph/gpio.h"
#include "periph/pwm.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the DSP0401 (for Nucleo-F411RE)
 * @{
 */
#ifndef DSP0401_PARAM_SDI_PIN
#define DSP0401_PARAM_SDI_PIN         GPIO_PIN(0, 10)  /* D2 */
#endif
#ifndef DSP0401_PARAM_CLK_PIN
#define DSP0401_PARAM_CLK_PIN         GPIO_PIN(1, 3)   /* D3 */
#endif
#ifndef DSP0401_PARAM_LAT_PIN
#define DSP0401_PARAM_LAT_PIN         GPIO_PIN(1, 5)   /* D4 */
#endif
#ifndef DSP0401_PARAM_PWM_DEV
#define DSP0401_PARAM_PWM_DEV         PWM_DEV(1)
#endif
#ifndef DSP0401_PARAM_PWM_CHAN
#define DSP0401_PARAM_PWM_CHAN        (0U)
#endif
#ifndef DSP0401_PARAM_BRIGHTNESS
#define DSP0401_PARAM_BRIGHTNESS      (255U)
#endif
#ifndef DSP0401_PARAM_MODULE_COUNT
#define DSP0401_PARAM_MODULE_COUNT    (1U)
#endif

#ifndef DSP0401_PARAMS
#define DSP0401_PARAMS                { .sdi          = DSP0401_PARAM_SDI_PIN,    \
                                        .clk          = DSP0401_PARAM_CLK_PIN,    \
                                        .lat          = DSP0401_PARAM_LAT_PIN,    \
                                        .pwm          = DSP0401_PARAM_PWM_DEV,    \
                                        .pwm_channel  = DSP0401_PARAM_PWM_CHAN,   \
                                        .brightness   = DSP0401_PARAM_BRIGHTNESS, \
                                        .module_count = DSP0401_PARAM_MODULE_COUNT }
#endif
/**@}*/

/**
 * @brief   Configure DSP0401
 */
static const dsp0401_params_t dsp0401_params[] =
{
    DSP0401_PARAMS,
};

#ifdef __cplusplus
}
#endif

/** @} */
