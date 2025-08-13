/*
 * SPDX-FileCopyrightText: 2019 Yegor Yefremov
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_esp32_ttgo-t-beam
 * @brief       Board specific configuration of direct mapped GPIOs
 * @file
 * @author      Yegor Yefremov <yegorslists@googlemail.com>
 * @{
 */

#include "board.h"
#include "saul/periph.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   LED configuration
 */
static const  saul_gpio_params_t saul_gpio_params[] =
{
#ifndef MODULE_ESP32_TTGO_T_BEAM_V1_0
    {
        .name = "LED",
        .pin = LED0_PIN,
        .mode = GPIO_OUT,
        .flags = SAUL_GPIO_INIT_CLEAR
    },
#endif
    {
        .name = "PRG",
        .pin = BTN0_PIN,
        .mode = BTN0_MODE,
        .flags = SAUL_GPIO_INVERTED
    },
};

#ifdef __cplusplus
}
#endif

/** @} */
