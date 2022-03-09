/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_common_nrf52xxxdk
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
 * @brief    Button configuration
 */
static const  saul_gpio_params_t saul_gpio_params[] =
{
#ifdef BTN0_PIN
    {
        .name  = "Button 1",
        .pin   = BTN0_PIN,
        .mode  = BTN0_MODE,
        .flags = SAUL_GPIO_INVERTED,
    },
#endif
#ifdef BTN1_PIN
    {
        .name  = "Button 2",
        .pin   = BTN1_PIN,
        .mode  = BTN1_MODE,
        .flags = SAUL_GPIO_INVERTED,
    },
#endif
#ifdef BTN2_PIN
    {
        .name  = "Button 3",
        .pin   = BTN2_PIN,
        .mode  = BTN2_MODE,
        .flags = SAUL_GPIO_INVERTED,
    },
#endif
#ifdef BTN3_PIN
    {
        .name  = "Button 4",
        .pin   = BTN3_PIN,
        .mode  = BTN3_MODE,
        .flags = SAUL_GPIO_INVERTED,
    },
#endif
};

#ifdef __cplusplus
}
#endif

#endif /* GPIO_PARAMS_H */
/** @} */
