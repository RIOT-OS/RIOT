/*
 * Copyright (C) 2021 Luo Jia (HUST IoT Security Lab)
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_alientek-pandora
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Alientek Pandora board
 *
 * @author      Luo Jia <luojia@hust.edu.cn>
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
#define LED0_PIN            GPIO_PIN(PORT_E, 7)  /**< LED0 gpio pin  */
#define LED0_MASK           (1 << 7)             /**< LED0 gpio mask */

#define LED0_ON             (GPIOB->BSRR = LED0_MASK)  /**< Turn on LED0  */
#define LED0_OFF            (GPIOB->BSRR = (LED0_MASK << 16))  /**< Turn off LED0  */
#define LED0_TOGGLE         (GPIOB->ODR  ^= LED0_MASK)  /**< Toggle LED0  */

#define LED1_PIN            GPIO_PIN(PORT_E, 8)  /**< LED1 gpio pin  */
#define LED1_MASK           (1 << 8)             /**< LED1 gpio mask */

#define LED1_ON             (GPIOE->BSRR = LED1_MASK)  /**< Turn on LED1  */
#define LED1_OFF            (GPIOE->BSRR = (LED1_MASK << 16))  /**< Turn off LED1  */
#define LED1_TOGGLE         (GPIOE->ODR  ^= LED1_MASK)  /**< Toggle LED1  */

#define LED2_PIN            GPIO_PIN(PORT_E, 9)  /**< LED2 gpio pin  */
#define LED2_MASK           (1 << 9)             /**< LED2 gpio mask */

#define LED2_ON             (GPIOE->BSRR = LED2_MASK)  /**< Turn on LED2  */
#define LED2_OFF            (GPIOE->BSRR = (LED2_MASK << 16))  /**< Turn off LED2  */
#define LED2_TOGGLE         (GPIOE->ODR  ^= LED2_MASK)  /**< Toggle LED2  */

/** @} */

/**
 * @name   Wake-up and key buttons
 * @{
 */
#define BTNWK_PIN           GPIO_PIN(PORT_C, 13) /**< Wake button pin  */
#define BTNWK_MODE          GPIO_IN_PD          /**< Wake button mode */

#define BTN0_PIN            GPIO_PIN(PORT_D, 10) /**< Button 0 pin   */
#define BTN0_MODE           GPIO_IN_PD          /**< Button 0 mode  */

#define BTN1_PIN            GPIO_PIN(PORT_D, 9) /**< Button 1 pin    */
#define BTN1_MODE           GPIO_IN_PD          /**< Button 1 mode   */

#define BTN2_PIN            GPIO_PIN(PORT_D, 8) /**< Button 2 pin    */
#define BTN2_MODE           GPIO_IN_PD          /**< Button 2 mode   */
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
