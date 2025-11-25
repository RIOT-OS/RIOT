/*
 * Copyright (C) 2016 Freie Universität Berlin
 *               2018 Matthew Blue <matthew.blue.neuro@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_mega-xplained
 * @{
 *
 * @file
 * @brief       Configuration of SAUL mapped GPIO pins
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 */

#ifndef GPIO_PARAMS_H
#define GPIO_PARAMS_H

#include "board.h"
#include "saul/periph.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   GPIO configuration
 */
static const  saul_gpio_params_t saul_gpio_params[] =
{
    {
        .name = "Button 0",
        .pin  = BTN0_PIN,
        .mode = BTN0_MODE,
        .flags = SAUL_GPIO_INVERTED,
    },
    {
        .name = "Button 1",
        .pin  = BTN1_PIN,
        .mode = BTN1_MODE,
        .flags = SAUL_GPIO_INVERTED,
    },
    /* BTN2, LED0,2 currently unsupported due to lack of GPIO_OD support */
    {
        .name = "LED 1",
        .pin = LED1_PIN,
        .mode = LED1_MODE,
        .flags = (SAUL_GPIO_INVERTED | SAUL_GPIO_INIT_CLEAR),
    },
    {
        .name = "LED 3",
        .pin = LED3_PIN,
        .mode = LED3_MODE,
        .flags = (SAUL_GPIO_INVERTED | SAUL_GPIO_INIT_CLEAR),
    }
};

#ifdef __cplusplus
}
#endif

#endif /* GPIO_PARAMS_H */
/** @} */
