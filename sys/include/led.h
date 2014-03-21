/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    sys_led LED Interface
 * @ingroup     sys
 * @brief       Simple interface to the borads LEDs
 * @{
 *
 * @file        led.c
 * @brief       Simple universal LED interface to interface with a bords leds.
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#ifndef __LED_H
#define __LED_H

#include "periph/gpio.h"

#ifndef LED_ON
#define LED_ON      (1)         /* set pin to 1 to turn LED on */
#endif
#ifndef LED_OFF
#define LED_OFF     (0)         /* set pin to 0 to turn LED off */
#endif


/**
 * @brief Initialize all LEDs on the board
 */
void led_init(gpio_t *pins, int numof);

/**
 * @brief Turn a given LED on
 * 
 * @param led Index of the LED to turn on
 */
void led_on(int led);

/**
 * @brief Turn a given LED off
 * 
 * @param led Index of the LED to turn off
 */
void led_off(int led);

/**
 * @brief Toggle a given LED
 * 
 * @param led Index of the LED to toggle
 */
void led_toggle(int led);

/**
 * @brief Set given led to given state
 * 
 * @param led Index of LED to set
 * @param state New state for led, 0 for off, 1 for on
 */
void led_set(int led, int state);

/**
 * @brief Get the current status of the given LED
 * 
 * @param led Index of the LED to read the status from
 * @return 1 if LED is on, 0 if LED is off, -1 if no LED with given index exists
 */
int led_get_status(int led);


#endif /* __LED_H */
/** @} */
