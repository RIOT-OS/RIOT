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
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    GPIO pin configuration
 */
static const  saul_gpio_params_t saul_gpio_params[] =
{
    {
        .pin = LED0_PIN,
        .mode = GPIO_OUT
    },
    {
        .pin = LED1_PIN,
        .mode = GPIO_OUT
    },
    {
        .pin = LED2_PIN,
        .mode = GPIO_OUT
    },
    {
        .pin = LED3_PIN,
        .mode = GPIO_OUT
    },
    {
        .pin = LED4_PIN,
        .mode = GPIO_OUT
    },
    {
        .pin = LED5_PIN,
        .mode = GPIO_OUT
    },
    {
        .pin = LED6_PIN,
        .mode = GPIO_OUT
    },
    {
        .pin = LED7_PIN,
        .mode = GPIO_OUT
    },
    {
        .pin  = BTN0_PIN,
        .mode = BTN0_MODE
    },
};

/**
 * @brief GPIO information for SAUL registry
 */
static const saul_reg_info_t saul_gpio_info[] =
{
    { .name = "LD3" },
    { .name = "LD4" },
    { .name = "LD5" },
    { .name = "LD6" },
    { .name = "LD7" },
    { .name = "LD8" },
    { .name = "LD9" },
    { .name = "LD10" },
    { .name = "BTN USER" }
};

#ifdef __cplusplus
}
#endif

#endif /* GPIO_PARAMS_H */
/** @} */
