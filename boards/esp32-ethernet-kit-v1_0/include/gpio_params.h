/*
 * SPDX-FileCopyrightText: 2018 Gunar Schorcht
 * SPDX-FileCopyrightText: 2020 Google LLC
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

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
