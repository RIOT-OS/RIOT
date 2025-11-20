/*
 * SPDX-FileCopyrightText: 2021 Benjamin Valentin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_zigduino
 * @{
 *
 * @file
 * @brief       Board specific configuration of direct mapped GPIOs
 *
 * @author      Benjamin Valentin <benpicco@googlemail.com>
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
        .name = "motion",
        .pin  = PIR_MOTION_PIN,
        .mode = PIR_MOTION_MODE,
    },
};

#ifdef __cplusplus
}
#endif

/** @} */
