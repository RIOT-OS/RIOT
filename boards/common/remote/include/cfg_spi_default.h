/*
 * SPDX-FileCopyrightText: 2014-2016 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2015 Zolertia SL
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_common_remote
 * @{
 *
 * @file
 * @brief       Common default SPI configuration for the RE-Mote board revision A
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Antonio Lignan <alinan@zolertia.com>
 * @author      Sebastian Meiling <s@mlng.net>
 */

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .num      = 0,
        .mosi_pin = GPIO_PIN(PORT_B, 1),
        .miso_pin = GPIO_PIN(PORT_B, 3),
        .sck_pin  = GPIO_PIN(PORT_B, 2),
        .cs_pin   = GPIO_PIN(PORT_B, 5)
    },
    {
        .num      = 1,
        .mosi_pin = GPIO_PIN(PORT_C, 5),
        .miso_pin = GPIO_PIN(PORT_C, 6),
        .sck_pin  = GPIO_PIN(PORT_C, 4),
        .cs_pin   = GPIO_PIN(PORT_A, 7)
    }
};

#define SPI_NUMOF           ARRAY_SIZE(spi_config)
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

/** @} */
