/*
 * Copyright (C) 2017 Hamburg University of Applied Sciences
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_soft_spi
 * @{
 *
 * @file
 * @brief       Software SPI configuration
 *
 * @author      Markus Blechschmidt <Markus.Blechschmidt@haw-hamburg.de>
 */

#ifndef SOFT_SPI_PARAMS_H
#define SOFT_SPI_PARAMS_H

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
 * @brief   Sotware SPI port descriptor array
 */
static soft_spi_conf_t soft_spi_config[] = {
    SOFT_SPI_PARAMS,
};

#ifdef __cplusplus
}
#endif

#endif /* SOFT_SPI_PARAMS_H */
/** @} */
