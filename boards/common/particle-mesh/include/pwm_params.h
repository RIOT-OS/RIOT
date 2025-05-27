/*
 * Copyright (C) 2020 Christian Amsüss <chrysn@fsfe.org>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
