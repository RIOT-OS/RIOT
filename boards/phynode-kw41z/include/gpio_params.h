/*
 * SPDX-FileCopyrightText: 2018 Eistec AB
 * SPDX-FileCopyrightText: 2018 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup   boards_phynode-kw41z
 * @{
 *
 * @file
 * @brief     Board specific configuration of direct mapped GPIOs
 *
 * @author    Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author    José Alamos <jose.alamos@haw-hamburg.de>
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
    /* There are 2 user controlled LEDs on board. An orange LED (D2) and an
     * RGB LED (D10). The RGB LED is configured with 3 LEDx macros. */
#ifdef LED0_PIN
    {
        .name = "D2 (Orange)",
        .pin = LED0_PIN,
        .mode = GPIO_OUT,
        .flags = (SAUL_GPIO_INVERTED | SAUL_GPIO_INIT_CLEAR),
    },
#endif /* LED0_PIN */
#ifdef LED1_PIN
    {
        .name = "D10 RGB (Red)",
        .pin = LED1_PIN,
        .mode = GPIO_OUT,
        .flags = (SAUL_GPIO_INVERTED | SAUL_GPIO_INIT_CLEAR),
    },
#endif /* LED1_PIN */
#ifdef LED2_PIN
    {
        .name = "D10 RGB (Green)",
        .pin = LED2_PIN,
        .mode = GPIO_OUT,
        .flags = (SAUL_GPIO_INVERTED | SAUL_GPIO_INIT_CLEAR),
    },
#endif /* LED1_PIN */
#ifdef LED3_PIN
    {
        .name = "D10 RGB (Blue)",
        .pin = LED3_PIN,
        .mode = GPIO_OUT,
        .flags = (SAUL_GPIO_INVERTED | SAUL_GPIO_INIT_CLEAR),
    },
#endif /* LED1_PIN */
    {
        .name = "S2 (Button)",
        .pin = BTN0_PIN,
        .mode = BTN0_MODE,
        .flags = (SAUL_GPIO_INVERTED),
    },
};

#ifdef __cplusplus
}
#endif

/** @} */
