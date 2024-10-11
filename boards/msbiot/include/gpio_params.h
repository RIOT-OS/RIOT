/*
 * Copyright (C) 2018 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_msbiot
 * @{
 *
 * @file
 * @brief       Board specific configuration of direct mapped GPIOs
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
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
        .name = "red LED",
        .pin = LED0_PIN,
        .mode = GPIO_OUT,
        .flags = (SAUL_GPIO_INVERTED | SAUL_GPIO_INIT_CLEAR)
    },
    {
        .name = "yellow LED",
        .pin = LED1_PIN,
        .mode = GPIO_OUT,
        .flags = (SAUL_GPIO_INVERTED | SAUL_GPIO_INIT_CLEAR)
    },
    {
        .name = "green LED",
        .pin = LED2_PIN,
        .mode = GPIO_OUT,
        .flags = (SAUL_GPIO_INVERTED | SAUL_GPIO_INIT_CLEAR)
    },
    {
        .name = "left button",
        .pin = BUTTON0_PIN,
        .mode = GPIO_IN,
        .flags = 0
    },
    {
        .name = "right button",
        .pin = BUTTON1_PIN,
        .mode = GPIO_IN,
        .flags = 0
    },
};

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* GPIO_PARAMS_H */
