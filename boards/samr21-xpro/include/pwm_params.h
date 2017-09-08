/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup   boards_samr21-xpro
 * @{
 *
 * @file
 * @brief     Board specific configuration of direct mapped PWM
 *
 * @author    Fracisco Acosta <francisco.acosta@inria.fr>
 */

#ifndef PWM_PARAMS_H
#define PWM_PARAMS_H

#include "board.h"
#include "periph/pwm.h"
#include "saul/periph.h"

#define MODE        PWM_LEFT
#define FREQU       (1000U)
#define STEPS       (1000U)

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    PWM pin configuration
 */
static const  saul_pwm_params_t saul_pwm_params[] =
{
    {
        .name = "PWM(PA06)",
        .line = PWM_DEV(0),
        .mode = MODE,
        .freq = FREQU,
        .res = STEPS,
        .channel = 0
    },
    {
        .name = "PWM(PA07)",
        .line = PWM_DEV(0),
        .mode = MODE,
        .freq = FREQU,
        .res = STEPS,
        .channel = 1
    },
    {
        .name = "PWM(PA16)",
        .line = PWM_DEV(1),
        .mode = MODE,
        .freq = FREQU,
        .res = STEPS,
        .channel = 0
    },
    {
        .name = "PWM(PA18)",
        .line = PWM_DEV(1),
        .mode = MODE,
        .freq = FREQU,
        .res = STEPS,
        .channel = 1
    },
    {
        .name = "PWM(PA19)",
        .line = PWM_DEV(1),
        .mode = MODE,
        .freq = FREQU,
        .res = STEPS,
        .channel = 2
    },
};

#ifdef __cplusplus
}
#endif

#endif /* PWM_PARAMS_H */
/** @} */
