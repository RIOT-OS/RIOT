/*
 * Copyright (C) 2023 Benjamin Valentin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_hip_badge
 * @brief       Board specific configuration of direct mapped GPIOs
 * @file
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @{
 */

#ifndef GPIO_PARAMS_H
#define GPIO_PARAMS_H

#include "board.h"
#include "saul/periph.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   LED and Button configuration
 */
static const  saul_gpio_params_t saul_gpio_params[] =
{
    {
        .name = "SW1",
        .pin = BTN0_PIN,
        .mode = BTN0_MODE,
        .flags = SAUL_GPIO_INVERTED
    },
    {
        .name = "SW2",
        .pin = BTN1_PIN,
        .mode = BTN1_MODE,
        .flags = SAUL_GPIO_INVERTED
    },
    {
        .name = "Boot",
        .pin = BTN2_PIN,
        .mode = BTN2_MODE,
        .flags = SAUL_GPIO_INVERTED
    },
};

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* GPIO_PARAMS_H */
