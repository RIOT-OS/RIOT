/*
 * Copyright (C) 2016 Leon George
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_cc2650stk CC2650STK
 * @ingroup     boards
 * @brief       SimpleLink™ CC2650 sensor tag
 * @{
 *
 * @file
 * @brief       Board configuration for the CC2650STK
 *
 * @author      Leon M. George <leon@georgemail.eu>
 */

#ifndef BOARD_H_
#define BOARD_H_

#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Xtimer configuration
 * @{
 */
#define XTIMER_MASK                 (0xFFFF0000)
#define XTIMER_SHIFT_ON_COMPARE     (7)
/** @} */

/**
 * @brief   On-board button configuration
 * @{
 */
#define BUTTON1_DIO         GPIO_PIN(0, 4)
#define BUTTON2_DIO         GPIO_PIN(0, 0)
/** @} */

/**
 * @brief   Macros for controlling the on-board LEDs
 * @{
 */
#define LED0_PIN            GPIO_PIN(0, 10)
#define LED1_PIN            GPIO_PIN(0, 15)

#define LED0_ON             gpio_set(LED0_PIN)
#define LED0_OFF            gpio_clear(LED0_PIN)
#define LED0_TOGGLE         gpio_toggle(LED0_PIN)

#define LED1_ON             gpio_set(LED1_PIN)
#define LED1_OFF            gpio_clear(LED1_PIN)
#define LED1_TOGGLE         gpio_toggle(LED1_PIN)
/** @} */

/**
 * @brief   Initialize board specific hardware, including clock, LEDs, and std-IO
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H_ */
/** @} */
