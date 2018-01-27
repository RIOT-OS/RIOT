/*
 * Copyright (C) 2017 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_acd52832
 * @{
 *
 * @file
 * @brief       Configuration of SAUL mapped GPIO pins
 *
 * @author      Dimitri Nahm <dimitri.nahm@haw-hamburg.de>
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
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
        .mode = GPIO_OUT,
        .flags = (SAUL_GPIO_INVERTED | SAUL_GPIO_INIT_CLEAR),
    },
        {
        .name = "Button 1",
        .pin  = BTN0_PIN,
        .mode = BTN0_MODE,
        .flags = SAUL_GPIO_INVERTED,
    }
};


#ifdef __cplusplus
}
#endif

#endif /* GPIO_PARAMS_H */
/** @} */
