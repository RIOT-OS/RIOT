/*
 * SPDX-FileCopyrightText: 2020 Christian Amsüss <chrysn@fsfe.org>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_nrf52840dongle
 * @{
 *
 * @file
 * @brief       Configuration of SAUL mapped PWM channels
 *
 * @author      Christian Amsüss <chrysn@fsfe.org>
 */

#include "board.h"
#include "saul/periph.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    The single LED exposed via SAUL
 */
static const saul_pwm_dimmer_params_t saul_pwm_dimmer_params[] =
{
    {
        .name    = "LD 1",
        .channel = { PWM_DEV(0), 0, SAUL_PWM_INVERTED },
    }
};

static const saul_pwm_rgb_params_t saul_pwm_rgb_params[] =
{
    {
        .name  = "LD 2",
        .channels = {
            { PWM_DEV(0), 1, SAUL_PWM_INVERTED },
            { PWM_DEV(0), 2, SAUL_PWM_INVERTED },
            { PWM_DEV(0), 3, SAUL_PWM_INVERTED }
        }
    }
};

#ifdef __cplusplus
}
#endif

/** @} */
