/*
 * Copyright (C) 2020 Benjamin Valentin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_bluepill-stm32f030c8
 * @{
 *
 * @file
 * @brief       Board specific configuration of direct mapped GPIOs
 *
 * @author      Benjamin Valentin <benpicco@googlemail.com>
 */

#ifndef GPIO_PARAMS_H
#define GPIO_PARAMS_H

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
        .name  = "LED",
        .pin   = LED0_PIN,
        .mode  = GPIO_OUT,
        .flags = SAUL_GPIO_INVERTED,
    },
};

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* GPIO_PARAMS_H */
