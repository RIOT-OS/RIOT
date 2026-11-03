/*
 * SPDX-FileCopyrightText: Inria 2016
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_common_nucleo
 * @{
 *
 * @file
 * @brief       Board specific configuration of direct mapped GPIOs
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @author      Sebastian Meiling <s@mlng.net>
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
    {
        .name = "LED(green)",
        .pin = LED0_PIN,
        .mode = GPIO_OUT
    },
#endif
    {
        .name = "Button(B1 User)",
        .pin  = BTN0_PIN,
        .mode = BTN0_MODE,
#if !defined(CPU_MODEL_STM32L433RC) && !defined(CPU_MODEL_STM32G431RB) && \
    !defined(CPU_MODEL_STM32G474RE) && !defined(CPU_MODEL_STM32G491RE)
        .flags = SAUL_GPIO_INVERTED
#endif
    },
};

#ifdef __cplusplus
}
#endif

/** @} */
