/*
 * Copyright (C) 2018 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup   boards_frdm-kl43z
 * @{
 *
 * @file
 * @brief     Board specific configuration of direct mapped GPIOs
 *
 * @author    Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#ifndef GPIO_PARAMS_H
#define GPIO_PARAMS_H

#include "board.h"
#include "saul/periph.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    LED configuration
 */
static const  saul_gpio_params_t saul_gpio_params[] =
{
    {
        .name = "LED1(green)",
        .pin = LED0_PIN,
        .mode = GPIO_OUT,
        .flags = SAUL_GPIO_INVERTED
    },
    {
        .name = "LED2(red)",
        .pin = LED1_PIN,
        .mode = GPIO_OUT,
        .flags = SAUL_GPIO_INVERTED
    },
    {
        .name = "SW1",
        .pin = GPIO_PIN(PORT_C, 3),
        .mode = GPIO_IN_PU
    },
};

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* GPIO_PARAMS_H */
