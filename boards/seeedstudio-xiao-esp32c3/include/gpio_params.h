/*
 * Copyright (C) 2025 David Picard
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef GPIO_PARAMS_H
#define GPIO_PARAMS_H

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
 * @brief   LED and Button configuration
 */
static const  saul_gpio_params_t saul_gpio_params[] =
{
    {
        .name = "BOOT",
        .pin = BTN0_PIN,
        .mode = BTN0_MODE,
        .flags = SAUL_GPIO_INVERTED
    },
};

#ifdef __cplusplus
}
#endif

#endif /* GPIO_PARAMS_H */
/** @} */
