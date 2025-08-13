/*
 * SPDX-FileCopyrightText: 2020 Benjamin Valentin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_common_e104-bt50xxa-tb
 * @{
 *
 * @file
 * @brief       Peripheral configuration for the E104-BT50xxA Test Board
 *
 * @author      Benjamin Valentin <benpicco@googlemail.com>
 *
 */

#include "periph_cpu.h"
#include "cfg_rtt_default.h"
#include "cfg_timer_default.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Clock configuration
 *          The E104-BT50xxA module does not have any external oscillators
 * @{
 */
#define CLOCK_HFCLK         (1)             /* external crystal */

/* LFCLK Source clock selection:*/
/* - CLOCK_LFCLKSRC_SRC_RC: internal RC oscillator
 * - CLOCK_LFCLKSRC_SRC_Xtal: 32.768 kHz crystal
 * - CLOCK_LFCLKSRC_SRC_Synth: derived from HFCLK */
#define CLOCK_LFCLK         (CLOCK_LFCLKSRC_SRC_RC) /**< LFCLK Source */
/** @} */

/**
 * @name    SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .dev  = NRF_SPIM0,
        .sclk = GPIO_PIN(0, 26),
        .mosi = GPIO_PIN(0, 27),
        .miso = GPIO_PIN(0, 28),
    },
};
#define SPI_NUMOF           ARRAY_SIZE(spi_config)
/** @} */

/**
 * @name    I2C configuration
 * @{
 */
static const i2c_conf_t i2c_config[] = {
    {
        .dev = NRF_TWIM0,
        .scl = GPIO_PIN(0, 6),
        .sda = GPIO_PIN(0, 7),
        .speed = I2C_SPEED_NORMAL
    }
};
#define I2C_NUMOF           ARRAY_SIZE(i2c_config)
/** @} */

/**
 * @name    UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    { /* Mapped to USB virtual COM port */
        .dev        = NRF_UARTE0,
        .rx_pin     = GPIO_PIN(0, 14),
        .tx_pin     = GPIO_PIN(0, 18),
#ifdef MODULE_PERIPH_UART_HW_FC
        .rts_pin    = GPIO_UNDEF,
        .cts_pin    = GPIO_UNDEF,
#endif
        .irqn       = UARTE0_UART0_IRQn,
    },
};

#define UART_0_ISR          (isr_uart0)

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
