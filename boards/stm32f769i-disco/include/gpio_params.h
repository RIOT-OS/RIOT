/*
 * SPDX-FileCopyrightText: 2017 OTA keys S.A.
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup   boards_stm32f769i-disco
 * @{
 *
 * @file
 * @brief     Board specific configuration of direct mapped GPIOs
 *
 * @author    Vincent Dupont <vincent@otakeys.com>
 * @author    Sebastian Meiling <s@mlng.net>
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
        .name = "LD1",
        .pin = LED0_PIN,
        .mode = GPIO_OUT
    },
    {
        .name = "LD2",
        .pin = LED1_PIN,
        .mode = GPIO_OUT
    },
    {
        .name = "LD3",
        .pin = LED2_PIN,
        .mode = GPIO_OUT
    },
    {
        .name = "LD4",
        .pin = LED3_PIN,
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
