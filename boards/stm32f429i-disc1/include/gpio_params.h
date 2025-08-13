/*
 * SPDX-FileCopyrightText: 2018 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup   boards_stm32f429i-disc1
 * @{
 *
 * @file
 * @brief     Board specific configuration of direct mapped GPIOs
 *
 * @author    Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#include "board.h"
#include "saul/periph.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    GPIO pin configuration
 */
static const  saul_gpio_params_t saul_gpio_params[] =
{
    {
        .name = "LD3 (green)",
        .pin = LED0_PIN,
        .mode = GPIO_OUT
    },
    {
        .name = "LD4 (red)",
        .pin = LED1_PIN,
        .mode = GPIO_OUT
    },
    {
        .name = "BTN USER",
        .pin  = BTN0_PIN,
        .mode = BTN0_MODE
    },
};

#ifdef __cplusplus
}
#endif

/** @} */
