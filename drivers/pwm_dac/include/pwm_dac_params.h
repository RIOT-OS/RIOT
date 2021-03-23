/*
 * Copyright (C) 2020 Christian Amsüss <chrysn@fsfe.org>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_pwm_dac
 *
 * @{
 * @file
 * @brief       Default configuration for DAC implemented based on PWM devices
 *
 * @author      Christian Amsüss <chrysn@fsfe.org>
 */

#ifndef PWM_DAC_PARAMS_H
#define PWM_DAC_PARAMS_H

#include "board.h"
#include "pwm_dac.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Default configuration parameters for a single DAC PWM device
 *
 * @{
 */
#ifndef PWM_DAC_PARAM_DEV
#define PWM_DAC_PARAM_DEV        (PWM_DEV(0))
#endif
#ifndef PWM_DAC_PARAM_MODE
#define PWM_DAC_PARAM_MODE       (PWM_LEFT)
#endif
#ifndef PWM_DAC_PARAM_FREQ
#define PWM_DAC_PARAM_FREQ       (250000)
#endif
#ifndef PWM_DAC_PARAM_RESBITS
#define PWM_DAC_PARAM_RESBITS    (6)
#endif
/** @} */

/**
 * @brief    Value of initialization parameters for DAC PWMs
 */
#ifndef PWM_DAC_PARAMS
#define PWM_DAC_PARAMS           { .dev  = PWM_DAC_PARAM_DEV, \
                                   .mode = PWM_DAC_PARAM_MODE, \
                                   .freq = PWM_DAC_PARAM_FREQ, \
                                   .resbits = PWM_DAC_PARAM_RESBITS }
#endif

static const pwm_dac_params_t pwm_dac_params[] = {
    PWM_DAC_PARAMS
};

#define DAC_NUMOF           ARRAY_SIZE(pwm_dac_params)

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PWM_DAC_PARAMS_H */
