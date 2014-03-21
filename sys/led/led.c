/**
 * Copyright (C) 2014, Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup sys_led
 * @{
 * 
 * @file    led.c
 * @brief   Simple universal LED access to control a boards LEDs
 * 
 * @author  Hauke Petersen <hauke.petersen@fu-berlin.de>
 * 
 * @}
 */

#include "led.h"
#include "board.h"
#include "periph/gpio.h"

void led_init(gpio_t *pins, int numof)
{
    for (int i = 0; i < numof; i++) {
        gpio_init_out(pins[i], GPIO_NOPULL);
    }
}

void led_on(int led)
{
    gpio_write(led, LED_ON);
}

void led_off(int led)
{
    gpio_write(led, LED_OFF);
}

void led_toggle(int led)
{
    gpio_toggle(led);
}

void led_set(int led, int state)
{
    gpio_write(led, (state & 0x01) << (1 - LED_ON));
}

int led_get_status(int led)
{
    int val = gpio_read(led);
    return ((val & 0x01) << (1 - LED_ON));
}
