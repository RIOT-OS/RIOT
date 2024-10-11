/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup   boards_common_iotlab
 * @{
 *
 * @file
 * @brief     Board specific configuration of direct mapped GPIOs
 *
 * @author    Hauke Petersen <hauke.petersen@fu-berlin.de>
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
        .name = "LED(red)",
        .pin = LED0_PIN,
        .mode = GPIO_OUT,
        .flags = SAUL_GPIO_INVERTED | SAUL_GPIO_INIT_CLEAR,
    },
    {
        .name = "LED(green)",
        .pin = LED1_PIN,
        .mode = GPIO_OUT,
        .flags = SAUL_GPIO_INVERTED | SAUL_GPIO_INIT_CLEAR,
    },
    {
        .name = "LED(orange)",
        .pin = LED2_PIN,
        .mode = GPIO_OUT,
        .flags = SAUL_GPIO_INVERTED | SAUL_GPIO_INIT_CLEAR,
    },
};

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* GPIO_PARAMS_H */
