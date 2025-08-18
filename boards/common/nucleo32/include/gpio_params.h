/*
 * SPDX-FileCopyrightText: 2017 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_common_nucleo32
 * @{
 *
 * @file
 * @brief       Board specific configuration of direct mapped GPIOs
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#include "board.h"
#include "saul/periph.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MODULE_PERIPH_INIT_LED0
#define SAUL_GPIO_NUMOF (0U)
#else
/**
 * @brief    GPIO pin configuration
 */
static const  saul_gpio_params_t saul_gpio_params[] =
{
    {
        .name = "LD3(green)",
        .pin = LED0_PIN,
        .mode = GPIO_OUT
    }
};
#endif /* MODULE_PERIPH_INIT_LED0 */

#ifdef __cplusplus
}
#endif

/** @} */
