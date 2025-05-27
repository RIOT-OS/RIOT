/*
 * Copyright (C)  2016 Freie Universität Berlin
 *                2016-2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     boards_common_arduino-mkr
 * @{
 *
 * @file
 * @brief       Configuration of CPU peripherals for Arduino MKR boards
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @author      Bumsik kim <kbumsik@gmail.com>
 */

#include "periph_cpu.h"
#include "periph_conf_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev      = &SERCOM5->USART,
        .rx_pin   = GPIO_PIN(PB,23),  /* ARDUINO_PIN_13, RX Pin */
        .tx_pin   = GPIO_PIN(PB,22),  /* ARDUINO_PIN_14, TX Pin */
#ifdef MODULE_PERIPH_UART_HW_FC
        .rts_pin  = GPIO_UNDEF,
        .cts_pin  = GPIO_UNDEF,
#endif
        .mux      = GPIO_MUX_D,
        .rx_pad   = UART_PAD_RX_3,
        .tx_pad   = UART_PAD_TX_2,
        .flags    = UART_FLAG_NONE,
        .gclk_src = SAM0_GCLK_MAIN,
    }
};

/* interrupt function name mapping */
#define UART_0_ISR          isr_sercom5

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

/**
 * @name SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .dev      = &SERCOM1->SPI,
        .miso_pin = GPIO_PIN(PA, 19),   /* ARDUINO_PIN_8, SERCOM1-MISO */
        .mosi_pin = GPIO_PIN(PA, 16),   /* ARDUINO_PIN_10, SERCOM1-MOSI */
        .clk_pin  = GPIO_PIN(PA, 17),   /* ARDUINO_PIN_9, SERCOM1-SCK */
        .miso_mux = GPIO_MUX_C,
        .mosi_mux = GPIO_MUX_C,
        .clk_mux  = GPIO_MUX_C,
        .miso_pad = SPI_PAD_MISO_3,
        .mosi_pad = SPI_PAD_MOSI_0_SCK_1,
        .gclk_src = SAM0_GCLK_MAIN,
#ifdef MODULE_PERIPH_DMA
        .tx_trigger = SERCOM1_DMAC_ID_TX,
        .rx_trigger = SERCOM1_DMAC_ID_RX,
#endif
    },
    {
        .dev      = &SERCOM2->SPI,
        .miso_pin = GPIO_PIN(PA, 15),
        .mosi_pin = GPIO_PIN(PA, 12),
        .clk_pin  = GPIO_PIN(PA, 13),
        .miso_mux = GPIO_MUX_C,
        .mosi_mux = GPIO_MUX_C,
        .clk_mux  = GPIO_MUX_C,
        .miso_pad = SPI_PAD_MISO_3,
        .mosi_pad = SPI_PAD_MOSI_0_SCK_1,
        .gclk_src = SAM0_GCLK_MAIN,
#ifdef MODULE_PERIPH_DMA
        .tx_trigger = SERCOM2_DMAC_ID_TX,
        .rx_trigger = SERCOM2_DMAC_ID_RX,
#endif
    }
};

#define SPI_NUMOF           ARRAY_SIZE(spi_config)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
