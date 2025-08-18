/*
 * SPDX-FileCopyrightText: 2017 DAI Labor Technische Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup   boards_firefly
 * @{
 *
 * @file
 * @brief     Board specific configuration of direct mapped GPIOs
 *
 * @author    Anon Mall <anon.mall@gt-arc.com>
 */

#include "board.h"
#include "saul/periph.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    GPIO pin configuration
 */
static const saul_gpio_params_t saul_gpio_params[] =
{
    {
        .name = "LED(red)",
        .pin = LED0_PIN,
        .mode = GPIO_OUT
    },
    {
        .name = "LED(green)",
        .pin = LED1_PIN,
        .mode = GPIO_OUT
    },
    {
        .name = "LED(blue)",
        .pin = LED2_PIN,
        .mode = GPIO_OUT
    },
    {
        .name = "Button(User)",
        .pin = BTN0_PIN,
        .mode = BTN0_MODE
    },
};

#ifdef __cplusplus
}
#endif

/** @} */
