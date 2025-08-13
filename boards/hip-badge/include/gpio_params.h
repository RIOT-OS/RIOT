/*
 * SPDX-FileCopyrightText: 2023 Benjamin Valentin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_hip_badge
 * @brief       Board specific configuration of direct mapped GPIOs
 * @file
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @{
 */

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
