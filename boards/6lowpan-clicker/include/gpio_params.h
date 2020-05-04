/*
 * Copyright (C) 2020 Francois Berder
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_6lowpan-clicker
 * @{
 *
 * @file
 * @brief       Configuration of SAUL mapped GPIO pins
 *
 * @author      Francois Berder <fberder@outlook.fr>
 */

#ifndef GPIO_PARAMS_H
#define GPIO_PARAMS_H

#include "board.h"
#include "saul/periph.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   GPIO configuration
 */
static const saul_gpio_params_t saul_gpio_params[] =
{
    {
        .name = "LD1",
        .pin = LED1_PIN,
        .mode = GPIO_OUT,
    },
    {
        .name = "LD2",
        .pin = LED2_PIN,
        .mode = GPIO_OUT,
    },
    {
        .name  = "T1",
        .pin   = BTN0_PIN,
        .mode  = BTN0_MODE,
    },
    {
        .name  = "T2",
        .pin   = BTN1_PIN,
        .mode  = BTN1_MODE,
    },
};

#ifdef __cplusplus
}
#endif

#endif /* GPIO_PARAMS_H */
/** @} */
