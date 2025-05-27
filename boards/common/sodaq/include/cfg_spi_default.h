/*
 * Copyright (C) 2016 Kees Bakker, SODAQ
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     boards_common_sodaq
 * @{
 *
 * @file
 * @brief       Default SPI configuration for SODAQ boards
 *
 * @author      Kees Bakker <kees@sodaq.com>
 */

#include <stdint.h>

#include "cpu.h"
#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .dev      = &SERCOM3->SPI,
        .miso_pin = GPIO_PIN(PA, 22),
        .mosi_pin = GPIO_PIN(PA, 20),
        .clk_pin  = GPIO_PIN(PA, 21),
        .miso_mux = GPIO_MUX_C,
        .mosi_mux = GPIO_MUX_D,
        .clk_mux  = GPIO_MUX_D,
        .miso_pad = SPI_PAD_MISO_0,
        .mosi_pad = SPI_PAD_MOSI_2_SCK_3,
        .gclk_src = SAM0_GCLK_MAIN,
#ifdef MODULE_PERIPH_DMA
        .tx_trigger = SERCOM3_DMAC_ID_TX,
        .rx_trigger = SERCOM3_DMAC_ID_RX,
#endif
    },
};

#define SPI_NUMOF           ARRAY_SIZE(spi_config)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
