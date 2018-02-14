/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_common_nucleo144
 * @{
 *
 * @file
 * @brief       Board specific configuration of direct mapped GPIOs
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @author      Sebastian Meiling <s@mlng.net>
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
        .name = "LD1(green)",
        .pin = LED0_PIN,
        .mode = GPIO_OUT
    },
    {
        .name = "LD2(blue)",
        .pin = LED1_PIN,
        .mode = GPIO_OUT
    },
    {
        .name = "LD3(red)",
        .pin = LED2_PIN,
        .mode = GPIO_OUT
    },
    {
        .name = "B1(User button)",
        .pin  = BTN0_PIN,
        .mode = BTN0_MODE
    }
};

#ifdef __cplusplus
}
#endif

#endif /* GPIO_PARAMS_H */
/** @} */
