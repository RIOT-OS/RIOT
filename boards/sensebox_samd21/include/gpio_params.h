/*
 * SPDX-FileCopyrightText: 2018 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_sensebox_samd21
 * @{
 *
 * @file
 * @brief       Board specific configuration of direct mapped GPIOs
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
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
        .name = "LED(Red)",
        .pin = LED0_PIN,
        .mode = GPIO_OUT
    },
    {
        .name = "LED(Green)",
        .pin = LED1_PIN,
        .mode = GPIO_OUT
    },
    {
        .name = "BTN",
        .pin = BTN0_PIN,
        .mode = BTN0_MODE
    }
};

#ifdef __cplusplus
}
#endif

/** @} */
