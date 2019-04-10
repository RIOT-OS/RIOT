/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_common_nrf52
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
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    LED configuration
 */
static const  saul_gpio_params_t saul_gpio_params[] =
{
    {
        .pin   = LED0_PIN,
        .mode  = GPIO_OUT,
        .flags = (SAUL_GPIO_INVERTED | SAUL_GPIO_INIT_CLEAR),
    },
#ifdef LED1_PIN
    {
        .pin   = LED1_PIN,
        .mode  = GPIO_OUT,
        .flags = (SAUL_GPIO_INVERTED | SAUL_GPIO_INIT_CLEAR),
    },
#endif
#ifdef LED2_PIN
    {
        .pin   = LED2_PIN,
        .mode  = GPIO_OUT,
        .flags = (SAUL_GPIO_INVERTED | SAUL_GPIO_INIT_CLEAR),
    },
#endif
#ifdef LED3_PIN
    {
        .pin   = LED3_PIN,
        .mode  = GPIO_OUT,
        .flags = (SAUL_GPIO_INVERTED | SAUL_GPIO_INIT_CLEAR),
    },
#endif
#ifdef BTN0_PIN
    {
        .pin   = BTN0_PIN,
        .mode  = BTN0_MODE,
        .flags = SAUL_GPIO_INVERTED,
    },
#endif
#ifdef BTN1_PIN
    {
        .pin   = BTN1_PIN,
        .mode  = BTN1_MODE,
        .flags = SAUL_GPIO_INVERTED,
    },
#endif
#ifdef BTN2_PIN
    {
        .pin   = BTN2_PIN,
        .mode  = BTN2_MODE,
        .flags = SAUL_GPIO_INVERTED,
    },
#endif
#ifdef BTN3_PIN
    {
        .pin   = BTN3_PIN,
        .mode  = BTN3_MODE,
        .flags = SAUL_GPIO_INVERTED,
    },
#endif
};

/**
 * @brief GPIO information for SAUL registry
 */
static const saul_reg_info_t saul_gpio_info[] =
{
    { .name = "LED 1" },
#ifdef LED1_PIN
    { .name = "LED 2" },
#endif
#ifdef LED2_PIN
    { .name = "LED 3" },
#endif
#ifdef LED3_PIN
    { .name = "LED 4" },
#endif
#ifdef BTN0_PIN
    { .name = "Button 1" },
#endif
#ifdef BTN1_PIN
    { .name = "Button 2" },
#endif
#ifdef BTN2_PIN
    { .name = "Button 3" },
#endif
#ifdef BTN3_PIN
    { .name = "Button 4" },
#endif
};

#ifdef __cplusplus
}
#endif

#endif /* GPIO_PARAMS_H */
/** @} */
