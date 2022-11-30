/*
 * Copyright (C) 2022 Universidade de São Paulo
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_pulga
 * @{
 *
 * @file
 * @brief       Configuration of SAUL mapped PWM channels
 *
 * @author      Geovane Fedrecheski <geonnave@gmail.com>
 */

#ifndef PWM_PARAMS_H
#define PWM_PARAMS_H

#include "board.h"
#include "saul/periph.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SAUL_PWM_NO_RGB /**< No RGB leds provided */

/**
 * @brief    LED configuration
 */
static const saul_pwm_dimmer_params_t saul_pwm_dimmer_params[] =
{
#ifdef LED0_PIN
    {
        .name    = "LED 1 (D1)",
        .channel = { PWM_DEV(0), 0, SAUL_PWM_REGULAR },
    },
#endif
#ifdef LED1_PIN
    {
        .name    = "LED 2 (D2)",
        .channel = { PWM_DEV(0), 1, SAUL_PWM_REGULAR },
    },
#endif
};

#ifdef __cplusplus
}
#endif

#endif /* PWM_PARAMS_H */
/** @} */

