/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     board_RIoTboard
 * @brief       Support for the RIoTboard.
 * @{
 *
 * @file        board.h
 * @brief       Board specific definitions for the RIoTboard.
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Leon George <l.george@fu-berlin>
 */

#ifndef __BOARD_H
#define __BOARD_H

#include "ccm_pll.h"
#include "gpio.h"

/**
 * Define the nominal CPU core clock in this board
 * TODO this value is taken from the u-boot-clock-report
 *
 * #define F_CPU               (792000000UL)
 */
#define F_CPU  (get_main_clock(IPG_CLK))

/**
 * @name LED pin definitions
 * @{
 */
#define LED1_PORT GPIO_PORT3
#define LED1_PIN 25
#define LED2_PORT GPIO_PORT5
#define LED2_PIN 2
/** @} */

/**
 * @name Macros for controlling the on-board LEDs.
 * @{
 */
#define LED_1_ON (gpio_set_level(LED1_PORT, LED1_PIN, GPIO_LOW_LEVEL))
#define LED_1_OFF (gpio_set_level(LED1_PORT, LED1_PIN, GPIO_HIGH_LEVEL))
#define LED_1_TOGGLE (gpio_set_level(LED1_PORT, LED1_PIN, gpio_get_level(LED1_PORT, LED1_PIN) ^ GPIO_HIGH_LEVEL))

#define LED_2_ON (gpio_set_level(LED2_PORT, LED2_PIN, GPIO_LOW_LEVEL))
#define LED_2_OFF (gpio_set_level(LED2_PORT, LED2_PIN, GPIO_HIGH_LEVEL))
#define LED_2_TOGGLE (gpio_set_level(LED2_PORT, LED2_PIN, gpio_get_level(LED2_PORT, LED2_PIN) ^ GPIO_HIGH_LEVEL))

/* for compatability to other boards */
#define LED_GREEN_ON        LED_1_ON
#define LED_GREEN_OFF       LED_1_OFF
#define LED_GREEN_TOGGLE    LED_1_TOGGLE
#define LED_RED_ON          LED_2_ON
#define LED_RED_OFF         LED_2_OFF
#define LED_RED_TOGGLE      LED_2_TOGLLE
/** @} */


/**
 * @brief Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#endif
