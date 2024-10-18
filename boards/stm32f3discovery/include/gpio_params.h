/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup   boards_stm32f3discovery
 * @{
 *
 * @file
 * @brief     Board specific configuration of direct mapped GPIOs
 *
 * @author    Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author    Sebastian Meiling <s@mlng.net>
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
        .name = "LD3",
        .pin = LED0_PIN,
        .mode = GPIO_OUT
    },
    {
        .name = "LD4",
        .pin = LED1_PIN,
        .mode = GPIO_OUT
    },
    {
        .name = "LD5",
        .pin = LED2_PIN,
        .mode = GPIO_OUT
    },
    {
        .name = "LD6",
        .pin = LED3_PIN,
        .mode = GPIO_OUT
    },
    {
        .name = "LD7",
        .pin = LED4_PIN,
        .mode = GPIO_OUT
    },
    {
        .name = "LD8",
        .pin = LED5_PIN,
        .mode = GPIO_OUT
    },
    {
        .name = "LD9",
        .pin = LED6_PIN,
        .mode = GPIO_OUT
    },
    {
        .name = "LD10",
        .pin = LED7_PIN,
        .mode = GPIO_OUT
    },
    {
        .name = "BTN USER",
        .pin  = BTN0_PIN,
        .mode = BTN0_MODE
    },
};

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* GPIO_PARAMS_H */
