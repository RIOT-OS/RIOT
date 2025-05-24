/*
 * Copyright (C) 2018 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup   boards_usb-kw41z
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
    /* There are three LEDs on board, but only two are software controllable,
     * and one is driven by the debugger interface unless hardware mods have
     * been applied. See boards/usb-kw41z/include/board.h */
#ifdef LED0_PIN
    {
        .name = "D2",
        .pin = LED0_PIN,
        .mode = GPIO_OUT,
        .flags = (SAUL_GPIO_INVERTED | SAUL_GPIO_INIT_CLEAR),
    },
#endif /* LED0_PIN */
#ifdef LED1_PIN
    {
        .name = "D3",
        .pin = LED1_PIN,
        .mode = GPIO_OUT,
        .flags = (SAUL_GPIO_INVERTED | SAUL_GPIO_INIT_CLEAR),
    },
#endif /* LED1_PIN */
    {
        .name = "SW1",
        .pin = BTN0_PIN,
        .mode = BTN0_MODE,
        .flags = (SAUL_GPIO_INVERTED),
    },
};

#ifdef __cplusplus
}
#endif

/** @} */
