/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
