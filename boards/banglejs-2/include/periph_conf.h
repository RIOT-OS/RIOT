/*
 * Copyright (C) 2019 Inria
 *               2022 Nicole Faerber, Siegen, Germany
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_banglejs2
 * @{
 *
 * @file
 * @brief       Peripheral configuration for the Bangle.js 2
 *
 * @author      Nicole Faerber <nicole.faerber@digitaluhr-manufactur.de>
 *
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "periph_cpu.h"
#include "cfg_clock_32_1.h"
#include "cfg_rtt_default.h"
#include "cfg_timer_default.h"
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
        // CST816S touschscreen
        .dev = NRF_TWIM0,
        .scl = TOUCH_SCL,
        .sda = TOUCH_SDA,
        .speed = I2C_SPEED_FAST
    },
    {   // accellerometer KX023
        .dev = NRF_TWIM1,
        .scl = ACCEL_SCL,
        .sda = ACCEL_SDA,
        .speed = I2C_SPEED_FAST
    },
    {   // pressure/temp sensor BMP280
        .dev = NRF_TWIM1,
        .scl = ATM_PRESSURE_SCL,
        .sda = ATM_PRESSURE_SDA,
        .speed = I2C_SPEED_FAST
    },
    {   // heart rate sensor
        .dev = NRF_TWIM1,
        .scl = HRM_SCL,
        .sda = HRM_SDA,
        .speed = I2C_SPEED_FAST
    },
    {   // magnetometer
        .dev = NRF_TWIM1,
        .scl = MAGN_SCL,
        .sda = MAGN_SDA,
        .speed = I2C_SPEED_FAST
    },
};

#define I2C_NUMOF           ARRAY_SIZE(i2c_config)
/** @} */

/**
 * @name    SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        // LPM013M126 LCD
        .dev  = NRF_SPIM2,
        .sclk = LCD_SCK,
        .mosi = LCD_MOSI,
        //.miso = GPIO_UNDEF,
        //.ppi = 0,
    },
    {
        // SPI flash
        .dev  = NRF_SPIM3,
        .sclk = SPI_FLASH_SCK,
        .mosi = SPI_FLASH_MOSI,
        .miso = SPI_FLASH_MISO,
        //.ppi = 0,
    }
};

#define SPI_NUMOF           ARRAY_SIZE(spi_config)
/** @} */

/**
 * @name    UART configuration
 * @{
 */
/**
 * @brief   UART configuration.
 */
static const uart_conf_t uart_config[] = {
    {
        // GNSS
        .dev        = NRF_UARTE0,
        .rx_pin     = GPS_TXD,
        .tx_pin     = GPS_RXD,
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
 * @name    PWM configuration
 * @{
 */
static const pwm_conf_t pwm_config[] = {
    // LCD backlight
    { NRF_PWM0, { LCD_BACKLIGHT, GPIO_UNDEF, GPIO_UNDEF, GPIO_UNDEF } },
    // LCD EXTCOM
    { NRF_PWM1, { LCD_EXTCOMIN, GPIO_UNDEF, GPIO_UNDEF, GPIO_UNDEF } },
    // vibration motor
    { NRF_PWM2, { VIBRATOR, GPIO_UNDEF, GPIO_UNDEF, GPIO_UNDEF } }
};
#define PWM_NUMOF           ARRAY_SIZE(pwm_config)

/** @} */


#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
