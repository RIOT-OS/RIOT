/*
 * Copyright (C) 2020 Marian Buschsieweke
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_serpente
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

#endif /* PWM_PARAMS_H */
/** @} */
