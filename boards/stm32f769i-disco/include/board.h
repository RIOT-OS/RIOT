/*
 * SPDX-FileCopyrightText: 2017 OTA keys S.A.
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_stm32f769i-disco
 * @{
 *
 * @file
 * @brief       Board specific definitions for the STM32F769I-DISCO board
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @author      Sebastian Meiling <s@mlng.net>
 */

#include "cpu.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Macros for controlling the on-board LEDs.
 *
 * LED0 is the red user LED LD1 and LED1 is the green user LED LD2. LED2 is
 * the green LED LD3 on Arduino D13 (PA12), which is shared with SPI2 SCK.
 * @{
 */
#define LED0_PIN_NUM        13
#define LED0_PORT           GPIO_PORT_J /**< GPIO port of LED 0 */
#define LED0_PORT_NUM       PORT_J

#define LED1_PIN_NUM        5
#define LED1_PORT           GPIO_PORT_J /**< GPIO port of LED 1 */
#define LED1_PORT_NUM       PORT_J

#define LED2_PIN_NUM        12
#define LED2_PORT           GPIO_PORT_A /**< GPIO port of LED 2 */
#define LED2_PORT_NUM       PORT_A
/** @} */

/**
 * @name User button
 * @{
 */
#define BTN0_PIN            GPIO_PIN(PORT_A, 0)
#define BTN0_MODE           GPIO_IN
/** @} */

/**
 * @name DSI display panel signals
 *
 * The reset line is shared between the panel and the FT6x06 touch
 * controller, which does not respond on I2C while in reset.
 * @{
 */
#define LCD_RST_PIN         GPIO_PIN(PORT_J, 15)    /**< Panel and touch reset (low active) */
/** @} */

/**
 * @name FT6x06 touch panel configuration
 *
 * The controller responds at the driver's default address 0x38 (verified on
 * the MB1166-A09 panel).
 * @{
 */
#define FT5X06_PARAM_I2C_DEV    I2C_DEV(1)              /**< I2C device */
#define FT5X06_PARAM_INT_PIN    GPIO_PIN(PORT_I, 13)    /**< Interrupt pin */
#define FT5X06_PARAM_XMAX       (800)                   /**< Max width */
#define FT5X06_PARAM_YMAX       (480)                   /**< Max height */
#define FT5X06_PARAM_TYPE       FT5X06_TYPE_FT6X06      /**< Device type */
/** @} */

#ifdef __cplusplus
}
#endif

#include "stm32_leds.h"

/** @} */
