/*
 * SPDX-FileCopyrightText: 2020 Scallog
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_olimexino-stm32
 * @{
 *
 * @file
 * @brief       Board specific configuration of direct mapped GPIOs
 *
 * @author      Corentin Vigourt <cvigourt@scallog.com>
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
        .name = "LED2",
        .pin = LED0_PIN,
        .mode = GPIO_OUT
    },
    {
        .name = "LED1",
        .pin = LED1_PIN,
        .mode = GPIO_OUT
    },
    {
        .name = "BUT",
        .pin = BTN0_PIN,
        .mode = BTN0_MODE
    },
};

#ifdef __cplusplus
}
#endif

/** @} */
