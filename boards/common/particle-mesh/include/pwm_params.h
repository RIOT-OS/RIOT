/*
 * SPDX-FileCopyrightText: 2020 Christian Amsüss <chrysn@fsfe.org>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_common_particle-mesh
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

#define SAUL_PWM_NO_DIMMER

static const saul_pwm_rgb_params_t saul_pwm_rgb_params[] =
{
    {
        .name  = "LED",
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

/** @} */
