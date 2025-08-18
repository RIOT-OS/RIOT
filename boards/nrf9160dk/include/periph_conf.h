/*
 * SPDX-FileCopyrightText: 2021 Mesotic SAS
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup    boards_nrf9160dk
 * @{
 *
 * @file
 * @brief       Peripheral configuration for the nRF9160DK
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 *
 */

#include "periph_cpu.h"

#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    I2C configuration
 * @{
 */
static const i2c_conf_t i2c_config[] = {
    {
        .dev = NRF_TWIM3_S,
        .scl = GPIO_PIN(0, 31),
        .sda = GPIO_PIN(0, 30),
        .speed = I2C_SPEED_NORMAL
    }
};
#define I2C_NUMOF           ARRAY_SIZE(i2c_config)
/** @} */

/**
 * @name    SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .dev  = NRF_SPIM2_S,
        .sclk = GPIO_PIN(0, 13),
        .mosi = GPIO_PIN(0, 11),
        .miso = GPIO_PIN(0, 12),
    }
};

#define SPI_NUMOF           ARRAY_SIZE(spi_config)
/** @} */

/**
 * @name   Timer configuration
 * @{
 */
static const timer_conf_t timer_config[] = {
    {
        .dev      = NRF_TIMER0_S,
        /* using last channel for timer_read(), so only 5 of 6 channels available */
        .channels = 5,
        .bitmode  = TIMER_BITMODE_BITMODE_32Bit,
        .irqn     = TIMER0_IRQn
    },
    {
        .dev      = NRF_TIMER1_S,
        /* using last channel for timer_read(), so only 5 of 6 channels available */
        .channels = 5,
        .bitmode  = TIMER_BITMODE_BITMODE_08Bit,
        .irqn     = TIMER1_IRQn
    },
};

#define TIMER_0_ISR         isr_timer0 /**< Timer0 IRQ*/
#define TIMER_1_ISR         isr_timer1 /**< Timer1 IRQ */

/** See @ref timer_init */
#define TIMER_0_MAX_VALUE 0xffffffff
/** See @ref timer_init */
#define TIMER_1_MAX_VALUE 0xffffffff

#define TIMER_NUMOF         ARRAY_SIZE(timer_config) /**< Timer configuration NUMOF */
/** @} */

/**
 * @name    UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev        = NRF_UARTE0_S,
        .rx_pin     = GPIO_PIN(0, 28),
        .tx_pin     = GPIO_PIN(0, 29),
#ifdef MODULE_PERIPH_UART_HW_FC
        .rts_pin    = GPIO_UNDEF,
        .cts_pin    = GPIO_UNDEF,
#endif
        .irqn       = UARTE0_SPIM0_SPIS0_TWIM0_TWIS0_IRQn,
    },
    {
        .dev        = NRF_UARTE1_S,
        .rx_pin     = GPIO_PIN(0, 0),
        .tx_pin     = GPIO_PIN(0, 1),
#ifdef MODULE_PERIPH_UART_HW_FC
        .rts_pin    = GPIO_UNDEF,
        .cts_pin    = GPIO_UNDEF,
#endif
        .irqn       = UARTE1_SPIM1_SPIS1_TWIM1_TWIS1_IRQn,
    },
};

#define UART_NUMOF          ARRAY_SIZE(uart_config) /**< UART configuration NUMOF */
/** @} */

/**
 * @name    Real time counter configuration
 * @{
 */
#ifndef RTT_DEV
#define RTT_DEV             (0)                 /**< NRF_RTC0_S */
#endif

#define RTT_MAX_VALUE       (0x00ffffff)         /**< 24bit */
#define RTT_MAX_FREQUENCY   (32768U)             /**< in Hz */
#define RTT_MIN_FREQUENCY   (8U)                 /**< in Hz */
#define RTT_CLOCK_FREQUENCY (32768U)             /**< in Hz, LFCLK*/

#ifndef RTT_FREQUENCY
#define RTT_FREQUENCY       (1024U)              /**< in Hz */
#endif
/** @} */

/**
 * @name   PWM configuration
 * @{
 */
static const pwm_conf_t pwm_config[] = {
    {
        .dev = NRF_PWM0_S,
        .pin = {
                    LED0_PIN,
                    LED1_PIN,
                    LED2_PIN,
                    LED3_PIN
               }
    },
};

#define PWM_NUMOF           ARRAY_SIZE(pwm_config)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
