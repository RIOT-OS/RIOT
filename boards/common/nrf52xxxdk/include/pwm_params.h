/*
 * Copyright (C) 2021 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_common_nrf52xxxdk
 * @{
 *
 * @file
 * @brief       Configuration of SAUL mapped PWM channels
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
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
        .name    = "LED 1",
        .channel = { PWM_DEV(0), 0, SAUL_PWM_INVERTED },
    },
#endif
#ifdef LED1_PIN
    {
        .name    = "LED 2",
        .channel = { PWM_DEV(0), 1, SAUL_PWM_INVERTED },
    },
#endif
#ifdef LED2_PIN
    {
        .name    = "LED 3",
        .channel = { PWM_DEV(0), 2, SAUL_PWM_INVERTED },
    },
#endif
#ifdef LED3_PIN
    {
        .name    = "LED 4",
        .channel = { PWM_DEV(0), 3, SAUL_PWM_INVERTED },
    },
#endif
};

#ifdef __cplusplus
}
#endif

#endif /* PWM_PARAMS_H */
/** @} */
