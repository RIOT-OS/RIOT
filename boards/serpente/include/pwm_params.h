/*
 * SPDX-FileCopyrightText: 2020 Marian Buschsieweke
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_serpente
 * @{
 *
 * @file
 * @brief       Configuration of SAUL mapped PWM channels
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#include "board.h"
#include "saul/periph.h"

#ifdef __cplusplus
extern "C" {
#endif

static const saul_pwm_rgb_params_t saul_pwm_rgb_params[] =
{
    {
        .name  = "LED",
        .channels = {
            { PWM_DEV(0), 3, SAUL_PWM_INVERTED },
            { PWM_DEV(0), 2, SAUL_PWM_INVERTED },
            { PWM_DEV(0), 4, SAUL_PWM_INVERTED }
        }
    }
};

#define SAUL_PWM_NO_DIMMER

#ifdef __cplusplus
}
#endif

/** @} */
