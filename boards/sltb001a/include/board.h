/*
 * Copyright (C) 2015-2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_sltb001a Silicon Labs SLTB001A starter kit
 * @ingroup     boards
 * @brief       Support for the Silicon Labs SLTB001A starter kit
 * @{
 *
 * @file
 * @brief       Board specific definitions for the SLTB001A starter kit
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"

#include "periph_conf.h"
#include "periph/gpio.h"
#include "periph/spi.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Xtimer configuration.
 * @note    The timer runs at 250 KHz to increase accuracy.
 * @{
 */
#define XTIMER_HZ           (250000UL)
#define XTIMER_SHIFT        (2)
#define XTIMER_WIDTH        (16)
/** @} */

/**
 * @brief   Push button pin definitions.
 * @{
 */
#define PB0_PIN             GPIO_PIN(PD, 14)
#define PB1_PIN             GPIO_PIN(PD, 15)
/** @} */

/**
 * @brief    LED pin definitions.
 * @{
 */
#define LED0_PIN            GPIO_PIN(PD, 12)
#define LED1_PIN            GPIO_PIN(PD, 11)
/** @} */

/**
 * @brief   Macros for controlling the on-board LEDs.
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
 * @brief   Pin for enabling environmental sensors (BMP280, Si1133, Si7021, Si7210A).
 * @{
 */
#define ENV_SENSE_PIC_ADDR  (0x01)
#define ENV_SENSE_PIC_BIT   (0)
/** @} */

/**
 * @brief   Connection to the on-board pressure sensor (BMP280).
 * @{
 */
#ifndef BMP280_ENABLED
#define BMP280_ENABLED      (0)
#endif
#define BMP280_I2C          (0)
/** @} */

/**
 * @brief   Connection to the on-board air quality/gas sensor (CCS811).
 * @{
 */
#ifndef CCS811_ENABLED
#define CCS811_ENABLED      (0)
#endif
#define CCS811_I2C          (0)
#define CCS811_PIC_ADDR     (0x03)
#define CCS811_PIC_EN_BIT   (0x00)
#define CCS811_PIC_WAKE_BIT (0x01)
/** @} */

/**
 * @brief   Connection to the on-board IMU sensor (ICM-20648).
 * @{
 */
#ifndef ICM20648_ENABLED
#define ICM20648_ENABLED    (0)
#endif
#define ICM20648_SPI        (0)
#define ICM20648_PIC_ADDR   (0x00)
#define ICM20648_PIC_EN_BIT (0x00)
/** @} */

/**
 * @brief   Pin for communication with the Power & Interrupt Controller.
 * @{
 */
#define PIC_INT_WAKE_PIN    GPIO_PIN(PD, 10)
#define PIC_I2C             (0)
#define PIC_I2C_ADDR        (0x90)
/** @} */

/**
 * @brief   Pins for the four RGB LEDs.
 * @{
 */
#ifndef RGB_LED1_ENABLED
#define RGB_LED1_ENABLED    (1)
#endif
#ifndef RGB_LED2_ENABLED
#define RGB_LED2_ENABLED    (1)
#endif
#ifndef RGB_LED3_ENABLED
#define RGB_LED3_ENABLED    (1)
#endif
#ifndef RGB_LED4_ENABLED
#define RGB_LED4_ENABLED    (1)
#endif
#define RGB_LED_ADDR        (0x04)
#define RGB_LED_EN_BIT      (0x00)
#define RGB_LED1_EN_BIT     (0x07)
#define RGB_LED2_EN_BIT     (0x06)
#define RGB_LED3_EN_BIT     (0x05)
#define RGB_LED4_EN_BIT     (0x04)
/** @} */

/**
 * @brief   Connection to the on-board UV/ambient light sensor (Si1133).
 * @{
 */
#ifndef SI1133_ENABLED
#define SI1133_ENABLED      (0)
#endif
#define SI1133_I2C          (0)
/** @} */

/**
 * @brief   Connection to the on-board temperature/humidity sensor (Si7021).
 * @{
 */
#ifndef SI7021_ENABLED
#define SI7021_ENABLED      (1)
#endif
#define SI7021_I2C          (0)
/** @} */

/**
 * @brief   Connection to the on-board hall-effect sensor (Si7210A).
 * @{
 */
#ifndef SI7210A_ENABLED
#define SI7210A_ENABLED      (0)
#endif
#define SI7210A_I2C          (0)
/** @} */

/**
 * @brief   Initialize the board (GPIO, sensors, clocks).
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
