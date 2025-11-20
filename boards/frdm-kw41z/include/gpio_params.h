/*
 * SPDX-FileCopyrightText: 2017 SKF AB
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup   boards_frdm-kw41z
 * @{
 *
 * @file
 * @brief     Board specific configuration of direct mapped GPIOs
 *
 * @author    Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

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
    /* These LEDs are marked on the board silkscreen with "LED3" for the red LED,
     * and "LED4" for the RGB LED, hence the names for the SAUL actuators don't
     * match the LEDx_PIN macros in board.h */
    /* LED1 and LED2 on the board are wired to the target CPU reset pin, and the
     * power supply line and are not software controllable */
    {
        .name = "LED3",
        .pin = LED0_PIN,
        .mode = GPIO_OUT,
        .flags = (SAUL_GPIO_INVERTED | SAUL_GPIO_INIT_CLEAR),
    },
    {
        .name = "LED4_R",
        .pin = LED1_PIN,
        .mode = GPIO_OUT,
        .flags = (SAUL_GPIO_INVERTED | SAUL_GPIO_INIT_CLEAR),
    },
    {
        .name = "LED4_G",
        .pin = LED2_PIN,
        .mode = GPIO_OUT,
        .flags = (SAUL_GPIO_INVERTED | SAUL_GPIO_INIT_CLEAR),
    },
    {
        .name = "LED4_B",
        .pin = LED3_PIN,
        .mode = GPIO_OUT,
        .flags = (SAUL_GPIO_INVERTED | SAUL_GPIO_INIT_CLEAR),
    },
    {
        .name = "SW3",
        .pin = BTN0_PIN,
        .mode = BTN0_MODE,
        .flags = (SAUL_GPIO_INVERTED),
    },
    {
        .name = "SW4",
        .pin = BTN1_PIN,
        .mode = BTN1_MODE,
        .flags = (SAUL_GPIO_INVERTED),
    },
};

#ifdef __cplusplus
}
#endif

/** @} */
