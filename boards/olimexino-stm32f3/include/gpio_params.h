/*
 * SPDX-FileCopyrightText: 2025 Marian Buschsieweke
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_olimexino-stm32f3
 * @{
 *
 * @file
 * @brief       Board specific configuration of direct mapped GPIOs
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@posteo.net>
 */

#include "board.h"
#include "saul/periph.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   GPIO pin configuration
 */
static const saul_gpio_params_t saul_gpio_params[] =
{
    {
        .name = "green LED",
        .pin = LED0_PIN,
        .mode = GPIO_OUT
    },
    {
        .name = "yellow LED",
        .pin = LED1_PIN,
        .mode = GPIO_OUT
    },
    {
        .name = "BOOT (SW1)",
        .pin = BTN0_PIN,
        .mode = BTN0_MODE
    },
};

#ifdef __cplusplus
}
#endif
/** @} */
