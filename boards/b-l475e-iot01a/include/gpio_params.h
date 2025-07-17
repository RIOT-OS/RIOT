/*
 * SPDX-FileCopyrightText: 2017 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup   boards_b-l475e-iot01a
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
#ifdef MODULE_PERIPH_INIT_LED0
    /* The LED pin is also used for SPI, so we enable it
       only if explicitly wanted by the user */
    {
        .name = "LD1",
        .pin = LED0_PIN,
        .mode = GPIO_OUT
    },
#endif
    {
        .name = "LD2",
        .pin = LED1_PIN,
        .mode = GPIO_OUT
    },
    {
        .name = "Button(B1 User)",
        .pin = BTN0_PIN,
        .mode = BTN0_MODE,
        .flags = SAUL_GPIO_INVERTED
    }
};

#ifdef __cplusplus
}
#endif

/** @} */
