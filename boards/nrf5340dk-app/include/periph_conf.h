/*
 * Copyright (C) 2023 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup    boards_nrf5340dk-app
 * @{
 *
 * @file
 * @brief       Peripheral configuration for the nRF5340DK-app
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
 * @name   Timer configuration
 * @{
 */
static const timer_conf_t timer_config[] = {
    {
        .dev      = NRF_TIMER0_S,
        .channels = 5,
        .bitmode  = TIMER_BITMODE_BITMODE_32Bit,
        .irqn     = TIMER0_IRQn
    },
    {
        .dev      = NRF_TIMER1_S,
        .channels = 5,
        .bitmode  = TIMER_BITMODE_BITMODE_32Bit,
        .irqn     = TIMER1_IRQn
    },
};

#define TIMER_0_ISR         isr_timer0 /**< Timer0 IRQ*/
#define TIMER_1_ISR         isr_timer1 /**< Timer1 IRQ */

#define TIMER_NUMOF         ARRAY_SIZE(timer_config) /**< Timer configuration NUMOF */
/** @} */

/**
 * @name    UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev        = NRF_UARTE0_S,
        .rx_pin     = GPIO_PIN(1, 0),
        .tx_pin     = GPIO_PIN(1, 1),
#ifdef MODULE_PERIPH_UART_HW_FC
        .rts_pin    = GPIO_UNDEF,
        .cts_pin    = GPIO_UNDEF,
#endif
        .irqn       = SERIAL0_IRQn,
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

/**
 * @name    SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .dev  = NRF_SPIM1_S,
        .sclk = GPIO_PIN(0, 17),
        .mosi = GPIO_PIN(0, 13),
        .miso = GPIO_PIN(0, 14),
    }
};

#define SPI_NUMOF           ARRAY_SIZE(spi_config)
/** @} */

/**
 *  * @name    I2C configuration
 *   * @{
 *    */
static const i2c_conf_t i2c_config[] = {
    {
        .dev = NRF_TWIM2_S,
        .scl = GPIO_PIN(1, 3),
        .sda = GPIO_PIN(1, 2),
        .speed = I2C_SPEED_NORMAL
    }
};
#define I2C_NUMOF           ARRAY_SIZE(i2c_config)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
