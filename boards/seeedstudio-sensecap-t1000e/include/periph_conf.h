/*
 * SPDX-FileCopyrightText: 2026 Baptiste Le Duc <baptiste.leduc@etik.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_seeedstudio-sensecap-t1000e
 * @{
 *
 * @file
 * @brief       Peripheral configuration for the SeeedStudio SenseCAP T1000-E
 *
 * @author      Baptiste Le Duc <baptiste.leduc@etik.com>
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
 *
 * UART_DEV(0): GNSS module (AG3335)
 * UART_DEV(1): spare / debug
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev        = NRF_UARTE0,
        .rx_pin     = GPIO_PIN(0, 14),
        .tx_pin     = GPIO_PIN(0, 13),
#ifdef MODULE_PERIPH_UART_HW_FC
        .rts_pin    = GPIO_UNDEF,
        .cts_pin    = GPIO_UNDEF,
#endif
        .irqn       = UARTE0_UART0_IRQn,
    },
    {
        .dev        = NRF_UARTE1,
        .rx_pin     = GPIO_PIN(0, 17),
        .tx_pin     = GPIO_PIN(0, 16),
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

/**
 * @name    SPI configuration
 *
 * SPI_DEV(0): LR1110 LoRa transceiver
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .dev  = NRF_SPIM2,
        .sclk = GPIO_PIN(0, 11),
        .mosi = GPIO_PIN(1, 9),
        .miso = GPIO_PIN(1, 8),
    },
};

#define SPI_NUMOF           ARRAY_SIZE(spi_config)
/** @} */

/**
 * @name    I2C configuration
 *
 * I2C_DEV(0): QMA6100P accelerometer
 * @{
 */
static const i2c_conf_t i2c_config[] = {
    {
        .dev   = NRF_TWIM0,
        .scl   = GPIO_PIN(0, 27),
        .sda   = GPIO_PIN(0, 26),
        .speed = I2C_SPEED_FAST,
    },
};

#define I2C_NUMOF           ARRAY_SIZE(i2c_config)
/** @} */

/**
 * @name    PWM configuration
 *
 * PWM_DEV(0) CH0: buzzer (P0.25)
 *
 * @note    Unused channels must be set to GPIO_UNDEF.
 * @{
 */
static const pwm_conf_t pwm_config[] = {
    {
        .dev = NRF_PWM0,
        .pin = {
            GPIO_PIN(0, 25),
            GPIO_UNDEF,
            GPIO_UNDEF,
            GPIO_UNDEF,
        },
    },
};

#define PWM_NUMOF           ARRAY_SIZE(pwm_config)
/** @} */

/**
 * @name    ADC configuration
 *
 * ADC_NUMOF is fixed by the nRF52 CPU (9 or 10 channels).
 * Relevant lines for this board:
 *   NRF52_AIN0 (P0.02): battery voltage
 *   NRF52_AIN5 (P0.29): light sensor
 *   NRF52_AIN7 (P0.31): NTC temperature sensor
 * @{
 */
/* ADC_NUMOF defined in cpu/nrf52/include/periph_cpu.h */
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
