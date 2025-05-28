/*
 * Copyright (C) 2020 Bruno Chianca <brunobcf@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     boards_adafruit-itsybitsy-nrf52
 * @{
 *
 * @file
 * @brief       Configuration of SAUL mapped GPIO pins for the Adafruit
 *              ItsyBitsy nRF52840
 *
 * @author      Bruno Chianca <brunobcf@gmail.com>
 */

#include "board.h"
#include "saul/periph.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    LED and button configuration for SAUL
 */
static const  saul_gpio_params_t saul_gpio_params[] =
{
    {
        .name  = "LD 1",
        .pin   = LED0_PIN,
        .mode  = GPIO_OUT,
        .flags = SAUL_GPIO_INVERTED | SAUL_GPIO_INIT_CLEAR,
    },
    {
        .name  = "SW 1",
        .pin   = BTN0_PIN,
        .mode  = GPIO_IN_PU,
        .flags = SAUL_GPIO_INVERTED,
    },
};

#ifdef __cplusplus
}
#endif

/** @} */
