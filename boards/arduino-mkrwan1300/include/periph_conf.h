/*
 * SPDX-FileCopyrightText: 2016 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2016-2018 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_arduino-mkrwan1300
 * @{
 *
 * @file
 * @brief       Configuration of CPU peripherals for Arduino MKRWAN1300 board
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
    },
    { /* LoRa module */
        .dev      = &SERCOM4->USART,
        .rx_pin   = GPIO_PIN(PA,15),
        .tx_pin   = GPIO_PIN(PA,12),
#ifdef MODULE_PERIPH_UART_HW_FC
        .rts_pin  = GPIO_UNDEF,
        .cts_pin  = GPIO_UNDEF,
#endif
        .mux      = GPIO_MUX_D,
        .rx_pad   = UART_PAD_RX_3,
        .tx_pad   = UART_PAD_TX_0,
        .flags    = UART_FLAG_NONE,
        .gclk_src = SAM0_GCLK_MAIN,
    },
};

/* interrupt function name mapping */
#define UART_0_ISR          isr_sercom5
#define UART_1_ISR          isr_sercom4

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
    }
};

#define SPI_NUMOF           ARRAY_SIZE(spi_config)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
