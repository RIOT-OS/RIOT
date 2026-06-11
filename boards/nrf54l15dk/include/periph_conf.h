/*
 * SPDX-FileCopyrightText: 2026 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup    boards_nrf54l15dk
 * @{
 *
 * @file
 * @brief       Peripheral configuration for the nRF54L15-DK
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
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
        .dev      = NRF_TIMER20,
        .channels = 5,
        .bitmode  = TIMER_BITMODE_BITMODE_32Bit,
        .irqn     = TIMER20_IRQn
    },
    {
        .dev      = NRF_TIMER21,
        .channels = 5,
        .bitmode  = TIMER_BITMODE_BITMODE_32Bit,
        .irqn     = TIMER21_IRQn
    },
};

#define TIMER_0_ISR         isr_timer20 /**< Timer0 IRQ */
#define TIMER_1_ISR         isr_timer21 /**< Timer1 IRQ */

#define TIMER_NUMOF         ARRAY_SIZE(timer_config) /**< Timer configuration NUMOF */
/** @} */

/**
 * @name    UART configuration
 *
 * UARTE20 is connected to the second virtual COM port of the on-board
 * J-Link debugger.
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev        = NRF_UARTE20,
        .rx_pin     = GPIO_PIN(1, 5),
        .tx_pin     = GPIO_PIN(1, 4),
#ifdef MODULE_PERIPH_UART_HW_FC
        .rts_pin    = GPIO_UNDEF,
        .cts_pin    = GPIO_UNDEF,
#endif
        .irqn       = SERIAL20_IRQn,
    },
};

#define UART_0_ISR          isr_serial20 /**< UART0 IRQ */

#define UART_NUMOF          ARRAY_SIZE(uart_config) /**< UART configuration NUMOF */
/** @} */

/**
 * @name    SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .dev  = NRF_SPIM22,
        .sclk = GPIO_PIN(1, 11),
        .mosi = GPIO_PIN(1, 6),
        .miso = GPIO_PIN(1, 7),
    }
};

#define SPI_0_ISR           isr_serial22 /**< SPI0 IRQ */

#define SPI_NUMOF           ARRAY_SIZE(spi_config) /**< SPI configuration NUMOF */
/** @} */

/**
 * @name    I2C configuration
 * @{
 */
static const i2c_conf_t i2c_config[] = {
    {
        .dev = NRF_TWIM21,
        .scl = GPIO_PIN(1, 15),
        .sda = GPIO_PIN(1, 16),
        .speed = I2C_SPEED_NORMAL
    }
};

#define I2C_0_ISR           isr_serial21 /**< I2C0 IRQ */

#define I2C_NUMOF           ARRAY_SIZE(i2c_config) /**< I2C configuration NUMOF */
/** @} */

/**
 * @name   PWM configuration
 *
 * The PWM2x instances can only drive pins of port P1, so only LED1 and LED3
 * can be used (LED0 and LED2 are connected to port P2).
 * @{
 */
static const pwm_conf_t pwm_config[] = {
    {
        .dev = NRF_PWM20,
        .pin = {
                    LED1_PIN,
                    LED3_PIN,
                    GPIO_UNDEF,
                    GPIO_UNDEF
               }
    },
};

#define PWM_NUMOF           ARRAY_SIZE(pwm_config) /**< PWM configuration NUMOF */
/** @} */

/**
 * @name    Real time counter configuration
 *
 * The RTT is based on the GRTC, a 52-bit counter with a fixed frequency of
 * 1 MHz (the lower 32 bit are exposed by the driver).
 * @{
 */
#define RTT_MAX_VALUE       (0xffffffff)         /**< 32bit of the 52bit counter */
#define RTT_CLOCK_FREQUENCY (1000000U)           /**< in Hz, SYSCOUNTER frequency */
#define RTT_MAX_FREQUENCY   (1000000U)           /**< in Hz, no prescaler available */
#define RTT_MIN_FREQUENCY   (1000000U)           /**< in Hz, no prescaler available */

#ifndef RTT_FREQUENCY
#  define RTT_FREQUENCY     (1000000U)           /**< in Hz, fixed by hardware */
#endif
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
