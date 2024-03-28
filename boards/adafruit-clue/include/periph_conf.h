/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_adafruit-clue
 * @{
 *
 * @file
 * @brief       Peripheral configuration for the Adafruit Clue board
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "periph_cpu.h"
#include "cfg_clock_32_0.h"
#include "cfg_rtt_default.h"
#include "cfg_timer_default.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    UART configuration
 * @{
 */
/**
 * @brief   UART configuration.
 */
static const uart_conf_t uart_config[] = {
    {
        .dev        = NRF_UARTE0,
        .rx_pin     = GPIO_PIN(0, 4),
        .tx_pin     = GPIO_PIN(0, 5),
#ifdef MODULE_PERIPH_UART_HW_FC
        .rts_pin    = GPIO_UNDEF,
        .cts_pin    = GPIO_UNDEF,
#endif
        .irqn       = UARTE0_UART0_IRQn,
    },
};

/**
 * @brief   UART 0 Interrupt Service Routine.
 */
#define UART_0_ISR          (isr_uart0)
/**
 * @brief   Number of UART peripherals.
 */
#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

/**
 * @name    I2C configuration
 * @{
 */
/**
 * @brief   I2C configuration.
 */
static const i2c_conf_t i2c_config[] = {
    {   /* External connectors */
        .dev = NRF_TWIM0,
        .scl = GPIO_PIN(0, 0),      /* D19 */
        .sda = GPIO_PIN(0, 30),     /* D20 */
        .speed = I2C_SPEED_NORMAL
    },
    {   /* On board sensors */
        .dev = NRF_TWIM1,
        .scl = GPIO_PIN(0, 25),
        .sda = GPIO_PIN(0, 24),
        .speed = I2C_SPEED_NORMAL
    },
};

/**
 * @brief   Number of I2C peripherals.
 */
#define I2C_NUMOF           (sizeof(i2c_config) / sizeof(i2c_config[0]))
/** @} */

/**
 * @name    SPI configuration
 * @{
 */
/**
 * @brief   SPI configuration.
 */
static const spi_conf_t spi_config[] = {
    {   /* External connectors */
        .dev  = NRF_SPIM0,
        .sclk = GPIO_PIN(0, 23),    /* D13 */
        .mosi = GPIO_PIN(0, 21),    /* D15 */
        .miso = GPIO_PIN(0, 22),    /* D14 */
    },
    {   /* TFT LCD screen */
        .dev  = NRF_SPIM1,
        .sclk = GPIO_PIN(0, 14),
        .mosi = GPIO_PIN(0, 15),
        .miso = GPIO_PIN(0, 0),
    },
};

/**
 * @brief   Number of SPI peripherals.
 */
#define SPI_NUMOF           ARRAY_SIZE(spi_config)
/** @} */

/**
 * @brief    PDM configuration
 * @{
 */
static const pdm_conf_t pdm_config = {
    .din_pin = GPIO_PIN(0, 0),
    .clk_pin = GPIO_PIN(0, 1),
};
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
