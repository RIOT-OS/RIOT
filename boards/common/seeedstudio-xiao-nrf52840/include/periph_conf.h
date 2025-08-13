/*
 * SPDX-FileCopyrightText: 2024 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_seeedstudio-xiao-nrf52840
 * @ingroup     boards_seeedstudio-xiao-nrf52840-sense
 * @{
 *
 * @file
 * @brief       Peripheral configuration for the Seeed Studio XIAO nRF52840
 *              and Seeed Studio XIAO nRF52840 Sense
 *
 * @author      Mikolai Gütschow <mikolai.guetschow@tu-dresden.de>
 *
 */

#include "periph_cpu.h"
#include "cfg_clock_32_1.h"
#include "cfg_rtt_default.h"
#include "cfg_timer_default.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev        = NRF_UARTE0,
        .rx_pin     = GPIO_PIN(1, 12),
        .tx_pin     = GPIO_PIN(1, 11),
#ifdef MODULE_PERIPH_UART_HW_FC
        .rts_pin    = GPIO_UNDEF,
        .cts_pin    = GPIO_UNDEF,
#endif
        .irqn       = UARTE0_UART0_IRQn,
    },
};                                                  /**< UART Peripheral Configuration Structure */

#define UART_0_ISR          (isr_uart0)             /**< Interrupt Service Routing for UART 0 */

#define UART_NUMOF          ARRAY_SIZE(uart_config) /**< Number of (preconfigured) UARTs */
/** @} */

/**
 * @name    SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .dev  = NRF_SPIM0,
        .sclk = GPIO_PIN(1, 13),
        .mosi = GPIO_PIN(1, 15),
        .miso = GPIO_PIN(1, 14),
    },
    {
        .dev  = NRF_SPIM1,
        .sclk = GPIO_PIN(0, 21),
        .mosi = GPIO_PIN(0, 20),
        .miso = GPIO_PIN(0, 24),
    }
};                                                  /**< SPI Peripheral Configuration Structure */

#define SPI_NUMOF           ARRAY_SIZE(spi_config)  /**< Number of (preconfigured) SPI Buses */
/** @} */

/**
 * @name    I2C configuration
 * @{
 */
static const i2c_conf_t i2c_config[] = {
    {
        .dev = NRF_TWIM1,
        .scl = GPIO_PIN(0, 5),
        .sda = GPIO_PIN(0, 4),
        .speed = I2C_SPEED_NORMAL
    },
#ifdef BOARD_SEEEDSTUDIO_XIAO_NRF52840_SENSE
    {   /* internal I2C bus for the IMU */
        .dev = NRF_TWIM0,
        .scl = GPIO_PIN(0, 27),
        .sda = GPIO_PIN(0, 7),
        .speed = I2C_SPEED_NORMAL
    }
#endif
};                                                  /**< I2C Peripheral Configuration Structure */

#define I2C_NUMOF           ARRAY_SIZE(i2c_config)  /**< Number of (preconfigured) I2C Buses */
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
