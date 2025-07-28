/*
 * SPDX-FileCopyrightText: 2019 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup   boards_p-l496g-cell02
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
 * @brief    GPIO pin configuration
 */
static const  saul_gpio_params_t saul_gpio_params[] =
{
    {
        .name = "LD3",
        .pin = LED0_PIN,
        .mode = GPIO_OUT,
    },
    {
        .name = "LD1",
        .pin = LED1_PIN,
        .mode = GPIO_OUT,
        .flags = (SAUL_GPIO_INVERTED | SAUL_GPIO_INIT_CLEAR),
    },
    {
        .name = "Joystick (Center)",
        .pin = BTN0_PIN,
        .mode = BTN0_MODE
    },
    {
        .name = "Joystick (Left)",
        .pin = BTN1_PIN,
        .mode = BTN1_MODE
    },
    {
        .name = "Joystick (Down)",
        .pin = BTN2_PIN,
        .mode = BTN2_MODE
    },
    {
        .name = "Joystick (Right)",
        .pin = BTN3_PIN,
        .mode = BTN3_MODE
    },
    {
        .name = "Joystick (Up)",
        .pin = BTN4_PIN,
        .mode = BTN4_MODE
    },
};

#ifdef __cplusplus
}
#endif

/** @} */
