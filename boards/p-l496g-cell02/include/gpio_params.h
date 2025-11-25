/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup   boards_p-l496g-cell02
 * @{
 *
 * @file
 * @brief     Board specific configuration of direct mapped GPIOs
 *
 * @author    Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef GPIO_PARAMS_H
#define GPIO_PARAMS_H

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

#endif /* GPIO_PARAMS_H */
/** @} */
