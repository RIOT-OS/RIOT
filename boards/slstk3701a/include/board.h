/*
 * Copyright (C) 2015-2020 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     boards_slstk3701a
 * @{
 *
 * @file
 * @brief       Board specific definitions for the SLSTK3701A starter kit
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 */

#include "cpu.h"
#include "periph_conf.h"
#include "periph/gpio.h"
#include "periph/spi.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Xtimer configuration
 *
 * The timer runs at 1000 kHz to increase accuracy, or at 32.768 kHz if
 * LETIMER is used.
 * @{
 */
#if IS_ACTIVE(CONFIG_EFM32_XTIMER_USE_LETIMER)
#define XTIMER_DEV          (TIMER_DEV(2))
#define XTIMER_HZ           (32768UL)
#define XTIMER_WIDTH        (16)
#else
#define XTIMER_DEV          (TIMER_DEV(0))
#define XTIMER_HZ           (1000000UL)
#define XTIMER_WIDTH        (32)
#endif
#define XTIMER_CHAN         (0)
/** @} */

/**
 * @name    Board controller configuration
 *
 * Define the GPIO pin to enable the BC, to allow serial communication
 * via the USB port.
 * @{
 */
#define BC_PIN              GPIO_PIN(PE, 1)
/** @} */

/**
 * @name    Push button pin definitions
 * @{
 */
#define PB0_PIN             GPIO_PIN(PC, 8)
#define PB1_PIN             GPIO_PIN(PC, 9)
/** @} */

/**
 * @name    LED pin definitions
 * @{
 */
#define LED0R_PIN           GPIO_PIN(PH, 10)
#define LED0G_PIN           GPIO_PIN(PH, 11)
#define LED0B_PIN           GPIO_PIN(PH, 12)
#define LED1R_PIN           GPIO_PIN(PH, 13)
#define LED1G_PIN           GPIO_PIN(PH, 14)
#define LED1B_PIN           GPIO_PIN(PH, 15)
#define LED0_PIN            LED0R_PIN
#define LED1_PIN            LED1R_PIN
/** @} */

/**
 * @name    Macros for controlling the on-board LEDs
 * @{
 */
#define LED0_ON             gpio_set(LED0_PIN)
#define LED0_OFF            gpio_clear(LED0_PIN)
#define LED0_TOGGLE         gpio_toggle(LED0_PIN)
#define LED1_ON             gpio_set(LED1_PIN)
#define LED1_OFF            gpio_clear(LED1_PIN)
#define LED1_TOGGLE         gpio_toggle(LED1_PIN)
/** @} */

/**
 * @name    Display configuration
 *
 * Connection to the on-board Sharp Memory LCD (LS013B7DH03).
 * @{
 */
#define DISP_SPI            SPI_DEV(0)
#define DISP_COM_PIN        GPIO_PIN(PA, 11)
#define DISP_CS_PIN         GPIO_PIN(PC, 14)
#define DISP_EN_PIN         GPIO_PIN(PA, 9)
/** @} */

/**
 * @name    Temperature sensor configuration
 *
 * Connection to the on-board temperature/humidity sensor (Si7021).
 * @{
 */
#define SI7021_I2C              I2C_DEV(2)
#define SI7021_EN_PIN           GPIO_PIN(PB, 3)

#define SI70XX_PARAM_I2C_DEV    SI7021_I2C
/** @} */

/**
 * @name    Core temperature sensor configuration
 *
 * Connection to the on-chip temperature sensor.
 * @{
 */
#define CORETEMP_ADC        ADC_LINE(0)
/** @} */

/**
 * @brief   Initialize the board (GPIO, sensors, clocks).
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

/** @} */
