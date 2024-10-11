/*
 * Copyright (C) 2019 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup   boards_common_saml1x
 * @{
 *
 * @file
 * @brief     Board specific configuration of direct mapped GPIOs
 *
 * @author    Dylan Laduranty <dylan.laduranty@mesotic.com>
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
        .name = "LED(orange)",
        .pin = LED0_PIN,
        .mode = GPIO_OUT,
        .flags = (SAUL_GPIO_INVERTED | SAUL_GPIO_INIT_CLEAR)
    },
    {
        .name = "Button(SW0)",
        .pin  = BTN0_PIN,
        .mode = BTN0_MODE,
        .flags = SAUL_GPIO_INVERTED
    },
};

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* GPIO_PARAMS_H */
