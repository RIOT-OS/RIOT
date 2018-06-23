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
        .name = "Button %u",
        .pin = BTN_PORT_PIN0,
        .pinlist = (1 << (BTN0_PIN - BTN_PORT_PIN0))
                   | (1 << (BTN1_PIN - BTN_PORT_PIN0)),
        .mode = BTN0_MODE,
        .flags = SAUL_GPIO_INVERTED,
    },
    /* BTN2, LED0,2 currently unsupported due to lack of GPIO_OD support */
    {
        .name = "LED %u",
        .pin = LED_PORT_PIN0,
        .pinlist = (1 << (LED1_PIN - LED_PORT_PIN0))
                   | (1 << (LED3_PIN - LED_PORT_PIN0)),
        .mode = LED1_MODE,
        .flags = (SAUL_GPIO_INVERTED | SAUL_GPIO_INIT_CLEAR),
    }
};

#ifdef __cplusplus
}
#endif

#endif /* GPIO_PARAMS_H */
/** @} */
