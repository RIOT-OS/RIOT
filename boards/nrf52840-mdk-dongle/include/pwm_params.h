/*
 * Copyright (C) 2020 Benjamin Valentin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_nrf52840-mdk-dongle
 * @{
 *
 * @file
 * @brief       Configuration of SAUL mapped PWM channels
 *
 * @author      Benjamin Valentin <benpicco@googlemail.com>
 */

#ifndef PWM_PARAMS_H
#define PWM_PARAMS_H

#include "board.h"
#include "saul/periph.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    No individual LED that needs dimming
 */
#define SAUL_PWM_NO_DIMMER

/**
 * @brief    The on-board RGB LED
 */
static const saul_pwm_rgb_params_t saul_pwm_rgb_params[] =
{
    {
        .name  = "RGB",
        .channels = {
            { PWM_DEV(0), 0, SAUL_PWM_INVERTED },
            { PWM_DEV(0), 1, SAUL_PWM_INVERTED },
            { PWM_DEV(0), 2, SAUL_PWM_INVERTED }
        }
    }
};

#ifdef __cplusplus
}
#endif

#endif /* PWM_PARAMS_H */
/** @} */
