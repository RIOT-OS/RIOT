/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_nrf52840dk
 * @{
 *
 * @file
 * @brief       Configuration of SAUL mapped GPIO pins
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
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
 * @brief    LED configuration
 */
static const  saul_gpio_params_t saul_gpio_params[] =
{
    {
        .name = "LED 1",
        .pin = LED0_PIN,
        .mode = GPIO_OUT
    },
    {
        .name = "LED 2",
        .pin = LED1_PIN,
        .mode = GPIO_OUT
    },
    {
        .name = "LED 3",
        .pin = LED2_PIN,
        .mode = GPIO_OUT
    },
    {
        .name = "LED 4",
        .pin = LED3_PIN,
        .mode = GPIO_OUT
    },
    {
        .name = "Button 1",
        .pin  = BTN0_PIN,
        .mode = BTN0_MODE
    },
    {
        .name = "Button 2",
        .pin  = BTN1_PIN,
        .mode = BTN1_MODE
    },
    {
        .name = "Button 3",
        .pin  = BTN2_PIN,
        .mode = BTN2_MODE
    },
    {
        .name = "Button 4",
        .pin  = BTN3_PIN,
        .mode = BTN3_MODE
    }
};


#ifdef __cplusplus
}
#endif

#endif /* GPIO_PARAMS_H */
/** @} */
