/*
 * Copyright (C) 2023 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef GPIO_PARAMS_H
#define GPIO_PARAMS_H

/**
 * @ingroup     boards_esp32c3_wemos_mini
 * @brief       Board specific configuration of direct mapped GPIOs
 * @file
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @{
 */

#include "board.h"
#include "saul/periph.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   LED and Button configuration
 */
static const  saul_gpio_params_t saul_gpio_params[] =
{
    {
        .name = "BOOT",
        .pin = BTN0_PIN,
        .mode = BTN0_MODE,
        .flags = SAUL_GPIO_INVERTED
    },
#ifdef LED0_PIN
    {
        .name = "LED0",
        .pin = LED0_PIN,
        .mode = GPIO_OUT,
        .flags = SAUL_GPIO_INIT_CLEAR
    },
#endif
};

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* GPIO_PARAMS_H */
