/*
 * SPDX-FileCopyrightText: 2016 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2016 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup   boards_common_arduino_zero
 * @{
 *
 * @file
 * @brief     Board specific configuration of direct mapped GPIOs
 *
 * @author    Hauke Petersen <hauke.petersen@fu-berlin.de>
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
        .name = "LED(orange)",
        .pin = LED0_PIN,
        .mode = GPIO_OUT
    },
};

#ifdef __cplusplus
}
#endif

/** @} */
