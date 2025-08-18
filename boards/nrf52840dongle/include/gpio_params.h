/*
 * SPDX-FileCopyrightText: 2019 Christian Amsüss <chrysn@fsfe.org>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_nrf52840dongle
 * @{
 *
 * @file
 * @brief       Configuration of SAUL mapped GPIO pins
 *
 * @author      Christian Amsüss <chrysn@fsfe.org>
 */

#include "board.h"
#include "saul/periph.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    LED and button configuration for SAUL
 */
static const  saul_gpio_params_t saul_gpio_params[] =
{
    {
        .name  = "SW 1",
        .pin   = BTN0_PIN,
        .mode  = GPIO_IN_PU,
        .flags = SAUL_GPIO_INVERTED,
    },
};

#ifdef __cplusplus
}
#endif

/** @} */
