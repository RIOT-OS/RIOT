/*
 * Copyright (C) 2015-2020 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_common_slwstk6000b
 * @{
 *
 * @file
 * @brief       Board specific definitions for the SLWSTK6000B starter kit
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 * @author      Kai Beckmann <kai.beckmann@hs-rm.de>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "periph_conf.h"
#include "periph/adc.h"
#include "periph/gpio.h"
#include "periph/spi.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Xtimer configuration
 *
 * The timer runs at 250 kHz to increase accuracy, or at 32.768 kHz if
 * LETIMER is used.
 * @{
 */
#if IS_ACTIVE(CONFIG_EFM32_XTIMER_USE_LETIMER)
#define XTIMER_DEV          (TIMER_DEV(1))
#define XTIMER_HZ           (32768UL)
#define XTIMER_WIDTH        (16)
#else
#define XTIMER_DEV          (TIMER_DEV(0))
#define XTIMER_HZ           (250000UL)
#define XTIMER_WIDTH        (16)
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
#define BC_PIN              MODULE_PIN_F5
/** @} */

/**
 * @name    Push button pin definitions
 * @{
 */
#define PB0_PIN             MODULE_PIN_F12
#define PB1_PIN             MODULE_PIN_F13
/** @} */

/**
 * @name    LED pin definitions
 * @{
 */
#define LED0_PIN            MODULE_PIN_F10
#define LED1_PIN            MODULE_PIN_F11
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
 * @name    Core temperature sensor configuration
 *
 * Connection to the on-chip temperature sensor.
 * @{
 */
#define CORETEMP_ADC        ADC_LINE(0)
/** @} */

/**
 * @name    Display configuration
 *
 * Connection to the on-board Sharp Memory LCD (LS013B7DH03).
 * @{
 */
#define DISP_SPI            SPI_DEV(0)
#define DISP_COM_PIN        MODULE_PIN_F18
#define DISP_CS_PIN         MODULE_PIN_F17
#define DISP_EN_PIN         MODULE_PIN_F14
/** @} */

/**
 * @name    Temperature sensor configuration
 *
 * Connection to the on-board temperature/humidity sensor (Si7021).
 * @{
 */
#define SI7021_I2C              I2C_DEV(0)
#define SI7021_EN_PIN           MODULE_PIN_P37

#define SI70XX_PARAM_I2C_DEV    SI7021_I2C
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* BOARD_H */
