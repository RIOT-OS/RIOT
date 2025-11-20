/*
 * SPDX-FileCopyrightText: 2017 Frits Kuipers
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup   boards_maple-mini
 * @{
 *
 * @file
 * @brief     Board specific configuration of direct mapped GPIOs
 *
 * @author    Frits Kuipers <frits.kuipers@gmail.com>
 * @author      Sebastian Meiling <s@mlng.net>
 */

#include "board.h"
#include "saul/periph.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    Button/LED configuration
 */
static const  saul_gpio_params_t saul_gpio_params[] =
{
    {
        .name = "LED",
        .pin = LED0_PIN,
        .mode = GPIO_OUT
    },
    {
        .name = "BUTTON",
        .pin  = BTN0_PIN,
        .mode = BTN0_MODE
    }
};

#ifdef __cplusplus
}
#endif

/** @} */
