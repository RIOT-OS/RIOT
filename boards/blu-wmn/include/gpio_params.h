/*
 * Copyright (C) 2016 Freie Universität Berlin
 *               2018 Matthew Blue <matthew.blue.neuro@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_blu-wmn
 * @{
 *
 * @file
 * @brief       Configuration of SAUL mapped GPIO pins
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
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
static const  saul_gpio_params_t saul_gpio_params[] =
{
    {
        .name = "VGPIO",
        .pin = VGPIO_EN,
        .mode = GPIO_OUT,
        .flags  = (SAUL_GPIO_INVERTED | SAUL_GPIO_INIT_CLEAR)
    }
};

#ifdef __cplusplus
}
#endif

#endif /* GPIO_PARAMS_H */
/** @} */
