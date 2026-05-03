/*
 * SPDX-FileCopyrightText: 2026 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_common_native
 * @{
 *
 * @file
 * @brief       Board specific configuration of direct mapped GPIOs
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 */

#include "board.h"
#include "saul/periph.h"

#ifdef __cplusplus
extern "C" {
#endif

#if IS_USED(MODULE_PERIPH_GPIO_MOCK)
/**
 * @brief    GPIO pin configuration
 *
 * This is only used when MODULE_PERIPH_GPIO_MOCK is enabled to support
 * arbitrary GPIO pin configurations for testing purposes. Other GPIO drivers
 * would rely on pre-defined GPIO pin configurations, which are more dynamic
 * for the native board.
 *
 * There is no feedback when using this configuration because it is only used
 * to provide some mock SAUL GPIO capabilities.
 */
static const saul_gpio_params_t saul_gpio_params[] =
{
    {
        .name = "LED 0 (mock)",
        .pin = GPIO_PIN(0, 0),
        .mode = GPIO_OUT
    },
    {
        .name = "LED 1 (mock)",
        .pin = GPIO_PIN(0, 1),
        .mode = GPIO_OUT
    }
};
#endif

#ifdef __cplusplus
}
#endif

/** @} */
