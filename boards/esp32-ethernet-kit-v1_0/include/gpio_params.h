/*
 * Copyright (C) 2018 Gunar Schorcht
 * Copyright (C) 2020 Google LLC
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef GPIO_PARAMS_H
#define GPIO_PARAMS_H

/**
 * @ingroup     boards_esp32_esp-ethernet-kit
 * @brief       Board specific configuration of direct mapped GPIOs
 * @file
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @author      Erik Ekman <eekman@google.com>
 * @{
 */

#include "board.h"
#include "saul/periph.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   LED and button configuration
 */
static const  saul_gpio_params_t saul_gpio_params[] =
{
#ifndef MODULE_ESP_ETH
    {
        .name = "BOOT",
        .pin = BTN0_PIN,
        .mode = BTN0_MODE,
        .flags = SAUL_GPIO_INVERTED
    },
#endif
};

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* GPIO_PARAMS_H */
