/*
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup   boards_microbit_v2
 * @{
 *
 * @file
 * @brief     Board specific configuration of direct mapped GPIOs
 *
 * @author    Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#include "board.h"
#include "saul/periph.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   GPIO pin configuration
 */
static const saul_gpio_params_t saul_gpio_params[] =
{
    {
        .name = "Button A",
        .pin  = BTN0_PIN,
        .mode = BTN0_MODE,
        .flags = SAUL_GPIO_INVERTED,
    },
    {
        .name = "Button B",
        .pin  = BTN1_PIN,
        .mode = BTN1_MODE,
        .flags = SAUL_GPIO_INVERTED,
    },
    {
        .name = "Touch Logo",
        .pin  = BTN2_PIN,
        .mode = BTN2_MODE,
        .flags = SAUL_GPIO_INVERTED,
    },
    {
        .name = "Ring 0",
        .pin  = BTN3_PIN,
        .mode = BTN3_MODE,
        .flags = SAUL_GPIO_INVERTED,
    },
    {
        .name = "Ring 1",
        .pin  = BTN4_PIN,
        .mode = BTN4_MODE,
        .flags = SAUL_GPIO_INVERTED,
    },
    {
        .name = "Ring 2",
        .pin  = BTN5_PIN,
        .mode = BTN5_MODE,
        .flags = SAUL_GPIO_INVERTED,
    },
};

#ifdef __cplusplus
}
#endif

/** @} */
