/*
 * Copyright (C) 2017 Travis Griggs <travisgriggs@gmail.com>
 * Copyright (C) 2017 Dan Evans <photonthunder@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup   boards_samd21-xpro
 * @{
 *
 * @file
 * @brief     Board specific configuration of direct mapped GPIOs
 *
 * @author    Travis Griggs <travisgriggs@gmail.com>
 * @author    Dan Evans <photonthunder@gmail.com>
 * @author    Sebastian Meiling <s@mlng.net>
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
        .name = "LED(orange)",
        .pin = LED0_PIN,
        .mode = GPIO_OUT
    },
    {
        .name = "Button(SW0)",
        .pin  = BTN0_PIN,
        .mode = BTN0_MODE
    },
};

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* GPIO_PARAMS_H */
