/*
 * SPDX-FileCopyrightText: 2017 Hamburg University of Applied Sciences
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_soft_spi
 * @{
 *
 * @file
 * @brief       Software SPI configuration
 *
 * @author      Markus Blechschmidt <Markus.Blechschmidt@haw-hamburg.de>
 */

#include "soft_spi.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef SOFT_SPI_PARAM_MISO
#define SOFT_SPI_PARAM_MISO         (GPIO_UNDEF)
#endif
#ifndef SOFT_SPI_PARAM_MOSI
#define SOFT_SPI_PARAM_MOSI         (GPIO_PIN(0, 0))
#endif
#ifndef SOFT_SPI_PARAM_CLK
#define SOFT_SPI_PARAM_CLK          (GPIO_PIN(0, 1))
#endif

#ifndef SOFT_SPI_PARAMS
#define SOFT_SPI_PARAMS     { .miso_pin = SOFT_SPI_PARAM_MISO, \
                              .mosi_pin = SOFT_SPI_PARAM_MOSI, \
                              .clk_pin  = SOFT_SPI_PARAM_CLK }
#endif

/**
 * @brief   Software SPI port descriptor array
 */
static soft_spi_conf_t soft_spi_config[] = {
    SOFT_SPI_PARAMS,
};

/**
 * @brief   Number of software SPI buses
 */
#define SOFT_SPI_NUMOF             ARRAY_SIZE(soft_spi_config)

#ifdef __cplusplus
}
#endif

/** @} */
