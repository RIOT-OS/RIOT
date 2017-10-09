/*
 * Copyright (C) 2017 Bumsik Kim <k.bumsik@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_samd21-xpro
 * @{
 *
 * @file
 * @brief       WINC1500 Xplained Pro configuration for SAMD21-XPRO board
 * @author      Bumsik Kim <k.bumsik@gmail.com>
*/

#ifndef WINC1500_PARAMS_H
#define WINC1500_PARAMS_H

#include "winc1500.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Board specific WINC1500 configuration
 */
static const winc1500_params_t winc1500_params[] =
{
    {
        .spi        = SPI_DEV(0),
        .cs_pin     = GPIO_PIN(PA, 5),
        .int_pin    = GPIO_PIN(PB, 4),
        .reset_pin  = GPIO_PIN(PB, 6),
        .en_pin     = GPIO_PIN(PB, 5),
        .wake_pin   = GPIO_PIN(PB, 7),
        .spi_clk    = SPI_CLK_10MHZ
    }
};

#ifdef __cplusplus
}
#endif

#endif /* WINC1500_PARAMS_H */
/** @} */
