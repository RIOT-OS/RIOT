/*
 * Copyright (C) 2016-2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_common_arduino_due
 * @{
 *
 * @file
 * @brief       Parameters for W5100 Arduino Ehternet Shields (v4 and v5)
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef W5100_PARAMS_H
#define W5100_PARAMS_H

#include "arduino_pinmap.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Configuration parameters for the W5100 driver
 * @{
 */
#ifndef W5100_PARAM_SPI
#define W5100_PARAM_SPI         (SPI_DEV(0))
#endif
#ifndef W5100_PARAM_SPI_CLK
#define W5100_PARAM_SPI_CLK     (SPI_CLK_5MHZ)
#endif
#ifndef W5100_PARAM_CS
#define W5100_PARAM_CS          (ARDUINO_PIN_10)
#endif
#ifndef W5100_PARAM_EVT
#define W5100_PARAM_EVT         (ARDUINO_PIN_2)
#endif
/** @} */

/**
 * @brief   W5100 configuration instance
 */
static const  w5100_params_t w5100_params[] = {
    {
        .spi = W5100_PARAM_SPI,
        .clk = W5100_PARAM_SPI_CLK,
        .cs  = W5100_PARAM_CS,
        .evt = W5100_PARAM_EVT
    },
};

#ifdef __cplusplus
}
#endif

#endif /* W5100_PARAMS_H */
/** @} */
