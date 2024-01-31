/*
 * Copyright (C) 2024 Marian Buschsieweke
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_generic_cc2538_cc2592_dk
 * @{
 *
 * @file
 * @brief       Definitions for the Generic CC2538-CC2592-DK Board
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@posteo.net>
 */
#ifndef GPIO_PARAMS_H
#define GPIO_PARAMS_H

#include "board.h"
#include "saul/periph.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief   Expose LEDs and buttons via SAUL
 */
static const saul_gpio_params_t saul_gpio_params[] =
    {
        {
            .name = "LED Green",
            .pin = LED_GREEN_PIN,
            .mode = GPIO_OUT,
        },
        {
            .name = "LED Red",
            .pin = LED_RED_PIN,
            .mode = GPIO_OUT,
        },
        {
            .name = "LED Yellow",
            .pin = LED_YELLOW_PIN,
            .mode = GPIO_OUT,
        },
        {
            .name = "LED Blue",
            .pin = LED_BLUE_PIN,
            .mode = GPIO_OUT,
        },
        {
            .name = "BTN Left",
            .pin = BTN_LEFT_PIN,
            .mode = BTN_LEFT_MODE,
            .flags = SAUL_GPIO_INVERTED,
        },
        {
            .name = "BTN Right",
            .pin = BTN_RIGHT_PIN,
            .mode = BTN_RIGHT_MODE,
            .flags = SAUL_GPIO_INVERTED,
        },
        {
            .name = "BTN Up",
            .pin = BTN_UP_PIN,
            .mode = BTN_UP_MODE,
            .flags = SAUL_GPIO_INVERTED,
        },
        {
            .name = "BTN Down",
            .pin = BTN_DOWN_PIN,
            .mode = BTN_DOWN_MODE,
            .flags = SAUL_GPIO_INVERTED,
        },
        {
            .name = "BTN Select",
            .pin = BTN_SELECT_PIN,
            .mode = BTN_DOWN_MODE,
            .flags = SAUL_GPIO_INVERTED,
        },
};

#ifdef __cplusplus
}
#endif

#endif /* GPIO_PARAMS_H */
/** @} */
