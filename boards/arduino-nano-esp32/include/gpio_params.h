/*
 * SPDX-FileCopyrightText: 2025 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_arduino_nano_esp32
 * @brief       Board specific configuration of direct mapped GPIOs
 * @file
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @{
 */

#include "board.h"
#include "saul/periph.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   LED and button configuration
 */
static const  saul_gpio_params_t saul_gpio_params[] =
{
#ifdef LED0_PIN
    {
        .name = "LED red",
        .pin = LED0_PIN,
        .mode = GPIO_OUT,
        .flags = SAUL_GPIO_INVERTED,
    },
#endif
#ifdef LED1_PIN
    {
        .name = "LED blue",
        .pin = LED1_PIN,
        .mode = GPIO_OUT,
        .flags = SAUL_GPIO_INVERTED,
    },
#endif
#ifdef LED2_PIN
    {
        .name = "LED green",
        .pin = LED2_PIN,
        .mode = GPIO_OUT,
        .flags = SAUL_GPIO_INVERTED,
    }
#endif
};

#ifdef __cplusplus
}
#endif

/** @} */
