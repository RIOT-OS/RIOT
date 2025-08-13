/*
 * Copyright (C) 2018 Inria
 *               2020 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     boards_waveshare_nrf52840_eval_kit
 * @{
 *
 * @file
 * @brief       Peripheral configuration for the Waveshare nRF52840 Eval Kit
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @author      Gunar Schorcht <gunar@schorcht.net>
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
 * @brief Enable the internal DC/DC converter
 */
#define NRF5X_ENABLE_DCDC

/**
 * @name    I2C configuration
 * @{
 */
static const i2c_conf_t i2c_config[] = {
    {
        .dev = NRF_TWIM1,
        .scl = GPIO_PIN(0, 27),
        .sda = GPIO_PIN(0, 26),
        .speed = I2C_SPEED_NORMAL
    },
    {
        .dev = NRF_TWIM0,
        .scl = GPIO_PIN(0, 22),
        .sda = GPIO_PIN(1, 0),
        .speed = I2C_SPEED_NORMAL
    }

};
#define I2C_NUMOF           ARRAY_SIZE(i2c_config)
/** @} */

/**
 * @name   PWM configuration
 * @{
 */
static const pwm_conf_t pwm_config[] = {
    {
        .dev = NRF_PWM0,
        .pin = {
                   GPIO_PIN(1, 4),
                   GPIO_PIN(1, 6),
                   GPIO_PIN(1, 7),
                   GPIO_PIN(1, 11)
               }
    },
    {
        .dev = NRF_PWM1,
        .pin = {
                   GPIO_PIN(0, 13),
                   GPIO_PIN(0, 14),
                   GPIO_PIN(1, 9),
                   GPIO_PIN(0, 16)
               }
    },
    {
        .dev = NRF_PWM2,
        .pin = {
                   GPIO_PIN(0, 15),
                   GPIO_UNDEF,
                   GPIO_UNDEF,
                   GPIO_UNDEF
               }
    }
};

#define PWM_NUMOF           ARRAY_SIZE(pwm_config)
/** @} */

/**
 * @name    SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .dev  = NRF_SPIM0,
        .sclk = GPIO_PIN(1, 15),
        .mosi = GPIO_PIN(1, 13),
        .miso = GPIO_PIN(1, 14),
    },
    {
        .dev  = NRF_SPIM1,
        .sclk = GPIO_PIN(0, 17),
        .mosi = GPIO_PIN(0, 24),
        .miso = GPIO_PIN(0, 20),
    }
};
#define SPI_NUMOF           ARRAY_SIZE(spi_config)
/** @} */

/**
 * @name    UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    { /* Mapped to USB virtual COM port */
        .dev        = NRF_UARTE0,
        .rx_pin     = GPIO_PIN(0, 8),
        .tx_pin     = GPIO_PIN(0, 6),
#ifdef MODULE_PERIPH_UART_HW_FC
        .rts_pin    = GPIO_PIN(0, 25),
        .cts_pin    = GPIO_PIN(0, 7),
#endif
        .irqn       = UARTE0_UART0_IRQn,
    },
    { /* Mapped to Arduino D0/D1 pins */
        .dev        = NRF_UARTE1,
        .rx_pin     = GPIO_PIN(1, 1),
        .tx_pin     = GPIO_PIN(1, 2),
#ifdef MODULE_PERIPH_UART_HW_FC
        .rts_pin    = GPIO_UNDEF,
        .cts_pin    = GPIO_UNDEF,
#endif
        .irqn       = UARTE1_IRQn,
    },
};

#define UART_0_ISR          (isr_uart0)
#define UART_1_ISR          (isr_uarte1)

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
