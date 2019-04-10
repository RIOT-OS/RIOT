/*
 * Copyright (C) Inria 2018
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup   boards_stm32l476g-disco
 * @{
 *
 * @file
 * @brief     Board specific configuration of direct mapped GPIOs
 *
 * @author    Alexandre Abadie <alexandre.abadie@inria.fr>
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
        .pin = BTN0_PIN,
        .mode = BTN0_MODE
    },
    {
        .pin = BTN1_PIN,
        .mode = BTN1_MODE
    },
    {
        .pin = BTN2_PIN,
        .mode = BTN2_MODE
    },
    {
        .pin = BTN3_PIN,
        .mode = BTN3_MODE
    },
    {
        .pin = BTN4_PIN,
        .mode = BTN4_MODE
    },
};

/**
 * @brief GPIO information for SAUL registry
 */
static const saul_reg_info_t saul_gpio_info[] =
{
    { .name = "LD4" },
    { .name = "LD5" },
    { .name = "Joystick (Center)" },
    { .name = "Joystick (Left)" },
    { .name = "Joystick (Down)" },
    { .name = "Joystick (Right)" },
    { .name = "Joystick (Up)" },
};

#ifdef __cplusplus
}
#endif

#endif /* GPIO_PARAMS_H */
/** @} */
