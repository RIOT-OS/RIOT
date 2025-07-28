/*
 * SPDX-FileCopyrightText: 2017 Thomas Stilwell <stilwellt@openlabs.co>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup   boards_openlabs-kw41z-mini
 * @{
 *
 * @file
 * @brief     Board specific configuration of direct mapped GPIOs
 *
 * @author    Thomas Stilwell <stilwellt@openlabs.co>
 */

#include "board.h"
#include "saul/periph.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    SAUL configuration
 */
static const saul_gpio_params_t saul_gpio_params[] =
{
    {
        .name = "LED",
        .pin = LED0_PIN,
        .mode = GPIO_OUT,
        .flags = SAUL_GPIO_INVERTED,
    },
};

#ifdef __cplusplus
}
#endif

/** @} */
