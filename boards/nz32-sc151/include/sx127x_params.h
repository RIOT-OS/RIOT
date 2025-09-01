/*
 * SPDX-FileCopyrightText: 2017 Inria Chile
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_nz32-sc151
 * @{
 *
 * @file
 * @brief       sx127x configuration for nz32-sc15 using modtronix inAir9
 *              LoRa module
 * @author      Francisco Molina <francisco.molina@inria.cl>
*/

#include "sx127x.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Board specific SX127X configuration
 */
static const sx127x_params_t sx127x_params[] =
{
    {
        .spi       = SPI_DEV(0),
        .nss_pin   = GPIO_PIN(PORT_C,  8),
        .reset_pin = GPIO_PIN(PORT_A,  9),
        .dio0_pin  = GPIO_PIN(PORT_B,  0),
        .dio1_pin  = GPIO_PIN(PORT_B,  1),
        .dio2_pin  = GPIO_PIN(PORT_C,  6),
        .dio3_pin  = GPIO_PIN(PORT_A, 10)
    }
};

#ifdef __cplusplus
}
#endif

/** @} */
