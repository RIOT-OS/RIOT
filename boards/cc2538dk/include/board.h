/*
 * Copyright (C) 2014 Loci Controls Inc.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_cc2538dk CC2538DK
 * @ingroup     boards
 * @brief       Support for the Texas Instruments CC2538DK board.
 * @{
 *
 * @file
 *
 * @author      Ian Martin <ian@locicontrols.com>
 */

#ifndef BOARD_H_
#define BOARD_H_

#include "cpu.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Define the nominal CPU core clock in this board
 */
#define F_CPU               XOSC32M_FREQ

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

#define LED_RED_ON          cc2538_gpio_set(LED_GREEN_GPIO)
#define LED_RED_OFF         cc2538_gpio_clear(LED_GREEN_GPIO)
#define LED_RED_TOGGLE      cc2538_gpio_toggle(LED_GREEN_GPIO)

#define LED_YELLOW_ON       cc2538_gpio_set(LED_YELLOW_GPIO)
#define LED_YELLOW_OFF      cc2538_gpio_clear(LED_YELLOW_GPIO)
#define LED_YELLOW_TOGGLE   cc2538_gpio_toggle(LED_YELLOW_GPIO)

#define LED_GREEN_ON        cc2538_gpio_set(LED_GREEN_GPIO)
#define LED_GREEN_OFF       cc2538_gpio_clear(LED_GREEN_GPIO)
#define LED_GREEN_TOGGLE    cc2538_gpio_toggle(LED_GREEN_GPIO)

#define LED_ORANGE_ON       cc2538_gpio_set(LED_ORANGE_GPIO)
#define LED_ORANGE_OFF      cc2538_gpio_clear(LED_ORANGE_GPIO)
#define LED_ORANGE_TOGGLE   cc2538_gpio_toggle(LED_ORANGE_GPIO)

/* Default to red if the color is not specified: */
#define LED_ON              LED_RED_ON
#define LED_OFF             LED_RED_OFF
#define LED_TOGGLE          LED_RED_TOGGLE
/** @} */

/**
 * @name Flash Customer Configuration Area (CCA) parameters
 * @{
 */

#ifndef UPDATE_CCA
#define UPDATE_CCA                1
#endif

#define CCA_BACKDOOR_ENABLE       1
#define CCA_BACKDOOR_PORT_A_PIN   3 /**< Select button */
#define CCA_BACKDOOR_ACTIVE_LEVEL 0 /**< Active low */
/** @} */

/**
 * @brief Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /** BOARD_H_ */
/** @} */
