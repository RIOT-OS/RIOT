/*
 * Copyright (C) 2015 Attilio Dona'
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_cc3200 CC32000 launchpad
 * @ingroup     boards
 * @brief       Support for the Texas Instruments CC3200 launchpad board.
 * @{
 *
 * @file
 *
 * @author      Attilio Dona' <@attiliodona>
 */

#ifndef BOARD_H_
#define BOARD_H

#include <stdint.h>
#include "cpu.h"
//#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CHANGE 4
#define FALLING 3
#define RISING 2
#define HIGH 0x1
#define LOW  0x0

#define RED_LED 29
#define GREEN_LED 10
#define YELLOW_LED 9
#define PUSH1 3
#define PUSH2 11


/**
 * Define the nominal CPU core clock in this board
 */
#define F_CPU               80000000

/**
 * Assign the hardware timer
 */
#define HW_TIMER            TIMER_0

/**
 * @name Define UART device and baudrate for stdio
 * @{
 */
#define STDIO               UART_0
#define STDIO_BAUDRATE      115200
#define STDIO_RX_BUFSIZE    (64U)
/** @} */













/**
 * @name Macros for controlling the on-board LEDs.
 * @{
 */
#define LED_RED_GPIO        GPIO_PC0 /**< Red LED GPIO pin */
#define LED_YELLOW_GPIO     GPIO_PC1 /**< Yellow LED GPIO pin */
#define LED_GREEN_GPIO      GPIO_PC2 /**< Green LED GPIO pin */
#define LED_ORANGE_GPIO     GPIO_PC3 /**< Orange LED GPIO pin */

#define LED_RED_ON          cc3200_gpio_set(LED_GREEN_GPIO)
#define LED_RED_OFF         cc3200_gpio_clear(LED_GREEN_GPIO)
#define LED_RED_TOGGLE      cc3200_gpio_toggle(LED_GREEN_GPIO)

#define LED_YELLOW_ON       cc3200_gpio_set(LED_YELLOW_GPIO)
#define LED_YELLOW_OFF      cc3200_gpio_clear(LED_YELLOW_GPIO)
#define LED_YELLOW_TOGGLE   cc3200_gpio_toggle(LED_YELLOW_GPIO)

#define LED_GREEN_ON        cc3200_gpio_set(LED_GREEN_GPIO)
#define LED_GREEN_OFF       cc3200_gpio_clear(LED_GREEN_GPIO)
#define LED_GREEN_TOGGLE    cc3200_gpio_toggle(LED_GREEN_GPIO)

#define LED_ORANGE_ON       cc3200_gpio_set(LED_ORANGE_GPIO)
#define LED_ORANGE_OFF      cc3200_gpio_clear(LED_ORANGE_GPIO)
#define LED_ORANGE_TOGGLE   cc3200_gpio_toggle(LED_ORANGE_GPIO)

/* Default to red if the color is not specified: */
#define LED_ON              LED_RED_ON
#define LED_OFF             LED_RED_OFF
#define LED_TOGGLE          LED_RED_TOGGLE
/** @} */













/**
 * @brief Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* BOARD_H_ */
/** @} */
