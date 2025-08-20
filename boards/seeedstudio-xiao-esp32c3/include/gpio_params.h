/*
 * SPDX-FileCopyrightText: 2025 David Picard
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_seeedstudio-xiao-esp32c3
 * @brief       Board specific configuration of direct mapped GPIOs
 * @file
 * @author      David Picard
 * @{
 */

#include "board.h"
#include "saul/periph.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Button configuration
 */
static const  saul_gpio_params_t saul_gpio_params[] =
{
    {
#ifdef MODULE_PERIPH_INIT_BUTTONS
        .name = "BOOT",
        .pin = BTN0_PIN,
        .mode = BTN0_MODE,
        .flags = SAUL_GPIO_INVERTED
#endif
    },
};

#ifdef __cplusplus
}
#endif

/** @} */
