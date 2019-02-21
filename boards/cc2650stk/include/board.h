/*
 * Copyright (C) 2016 Leon George
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_cc2650stk
 * @{
 *
 * @file
 * @brief       Board configuration for the CC2650STK
 *
 * @author      Leon M. George <leon@georgemail.eu>
 * @author      Sebastian Meiling <s@mlng.net>
 */

#ifndef BOARD_H
#define BOARD_H

#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    xtimer configuration
 * @{
 */
#define XTIMER_WIDTH        (16)
#define XTIMER_BACKOFF      (50)
#define XTIMER_ISR_BACKOFF  (40)
/** @} */

/**
 * @name    On-board button configuration
 * @{
 */
#define BTN0_PIN            GPIO_PIN(0, 4)
#define BTN0_MODE           GPIO_IN_PU
#define BTN1_PIN            GPIO_PIN(0, 0)
#define BTN1_MODE           GPIO_IN_PU
/** @} */

/**
 * @name    Macros for controlling the on-board LEDs
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

#endif /* BOARD_H */
/** @} */
