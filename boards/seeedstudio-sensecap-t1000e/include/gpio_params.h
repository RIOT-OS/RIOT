/*
 * SPDX-FileCopyrightText: 2026 Baptiste Le Duc <baptiste.leduc@etik.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_seeedstudio-sensecap-t1000e
 * @{
 *
 * @file
 * @brief       Configuration of SAUL mapped GPIO pins
 *
 * @author      Baptiste Le Duc <baptiste.leduc@etik.com>
 */

#include "board.h"
#include "saul/periph.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   GPIO SAUL configuration
 */
static const saul_gpio_params_t saul_gpio_params[] =
{
    {
        .name  = "LED0",
        .pin   = LED0_PIN,
        .mode  = GPIO_OUT,
        .flags = SAUL_GPIO_INIT_CLEAR,
    },
    {
        .name  = "BTN0",
        .pin   = BTN0_PIN,
        .mode  = BTN0_MODE,
    },
};

#ifdef __cplusplus
}
#endif

/** @} */
