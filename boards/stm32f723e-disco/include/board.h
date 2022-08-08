/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_stm32f723e-disco
 * @{
 *
 * @file
 * @brief       Board specific definitions for the STM32F723E-DISCO board
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Macros for controlling the on-board LEDs.
 * @{
 */
#define LED0_PIN_NUM        5
#define LED0_PORT_NUM       PORT_A

#define LED1_PIN_NUM        7
#define LED1_PORT_NUM       PORT_A

#define LED2_PIN_NUM        1
#define LED2_PORT_NUM       PORT_B
/** @} */

/**
 * @name User button
 * @{
 */
#define BTN0_PIN            GPIO_PIN(PORT_A, 0)
#define BTN0_MODE           GPIO_IN
/** @} */

/**
 * @name FT5X06 touch panel configuration
 * @{
 */
#define FT5X06_PARAM_I2C_DEV            I2C_DEV(1)              /**< I2C device */
#define FT5X06_PARAM_INT_PIN            GPIO_PIN(PORT_I, 9)     /**< Interrupt pin */
#define FT5X06_PARAM_XMAX               (240)                   /**< Max width */
#define FT5X06_PARAM_YMAX               (240)                   /**< Max height */
#define FT5X06_PARAM_TYPE               FT5X06_TYPE_FT6X06      /**< Device type */
/** @} */

#ifdef __cplusplus
}
#endif

#include "stm32_leds.h"

#endif /* BOARD_H */
/** @} */
