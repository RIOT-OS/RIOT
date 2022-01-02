/*
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_b-u585i-iot02a
 * @{
 *
 * @file
 * @brief       Board specific definitions for the ST B-U585I-IOT02A board
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#define LED0_PIN            GPIO_PIN(PORT_E, 13)
#define LED0_MASK           (1 << 13)

#define LED0_ON             (GPIOE->BSRR = LED0_MASK)
#define LED0_OFF            (GPIOE->BSRR = (LED0_MASK << 16))
#define LED0_TOGGLE         (GPIOE->ODR  ^= LED0_MASK)

#define LED1_PIN            GPIO_PIN(PORT_H, 6)
#define LED1_MASK           (1 << 6)

#define LED1_ON             (GPIOH->BSRR = LED1_MASK)
#define LED1_OFF            (GPIOH->BSRR = (LED1_MASK << 16))
#define LED1_TOGGLE         (GPIOH->ODR  ^= LED1_MASK)

#define LED2_PIN            GPIO_PIN(PORT_H, 7)
#define LED2_MASK           (1 << 7)

#define LED2_ON             (GPIOH->BSRR = LED2_MASK)
#define LED2_OFF            (GPIOH->BSRR = (LED2_MASK << 16))
#define LED2_TOGGLE         (GPIOH->ODR  ^= LED2_MASK)
/** @} */

/**
 * @name    Button pin configuration
 * @{
 */
#define BTN0_PIN            GPIO_PIN(PORT_C, 13)
#define BTN0_MODE           GPIO_IN_PU
/** @} */

/**
 * @name    HTS221 temperature/humidity sensor configuration
 * @{
 */
#define HTS221_PARAM_I2C    I2C_DEV(1)
/** @} */

/**
 * @name    LPS22HH pressure/temperature sensor configuration
 * @{
 */
#define LPSXXX_PARAM_I2C    I2C_DEV(1)
/** @} */

/**
 * @brief   Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
