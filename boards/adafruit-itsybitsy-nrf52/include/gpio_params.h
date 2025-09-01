/*
 * SPDX-FileCopyrightText: 2020 Bruno Chianca <brunobcf@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
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
