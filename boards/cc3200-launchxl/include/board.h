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
 * @author      Attilio Dona'
 */

#ifndef BOARD_H_
#define BOARD_H_

#include <stdint.h>
#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * for requesting an IRQ when PIN value changes
 */
#define CHANGE 4

/**
 * for requesting an IRQ when PIN value goes high to low
 */
#define FALLING 3

/**
 * for requesting an IRQ when PIN value goes low to high
 */
#define RISING 2

/**
 * for requesting an IRQ when PIN value goes high
 */
#define HIGH 1

/**
 * for requesting an IRQ when PIN value goes low
 */
#define LOW  0

/**
 * red led id
 */
#define RED_LED 29

/**
 * green led id
 */
#define GREEN_LED 10

/**
 * yellow led id
 */
#define YELLOW_LED 9

/**
 * SW2 button id
 */
#define PUSH1 3

/**
 * SW3 button id
 */
#define PUSH2 11

/**
 * P1.10 on launchpad (bottom pin on left side)
 */
#define DEBUG_PIN 10

/**
 * @name Macro for easy porting of TI examples
 * @{
 */
#define GPIO_IF_LedOn     gpio_set
#define GPIO_IF_LedOff    gpio_clear
#define GPIO_IF_LedStatus gpio_read

#define MCU_RED_LED_GPIO    RED_LED
#define MCU_GREEN_LED_GPIO  GREEN_LED

/**
 * sometimes I suspect to be daltonic
 */
#define MCU_ORANGE_LED_GPIO YELLOW_LED

#define Report printf

/** @} */



/**
 * Define the nominal CPU core clock in this board
 */
#define F_CPU               80000000

/**
 * hardware timers modules
 */
#define TIMER_NUM_CHANNELS 4

/**
 * timer module allocated for xtimer
 */
#define XTIMER  TIMER_0

/**
 * xtimer fixed channel value
 */
#define XTIMER_CHAN (0)


#if TIMER_0_MAX_VALUE == 0xffffff
#define XTIMER_MASK 0xff000000
#elif TIMER_0_MAX_VALUE == 0xffff
#define XTIMER_MASK 0xffff0000
#endif

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

#define LED_RED_ON          gpio_set(RED_LED)
#define LED_RED_OFF         gpio_clear(RED_LED)
#define LED_RED_TOGGLE      gpio_toggle(RED_LED)

#define LED_YELLOW_ON       gpio_set(YELLOW_LED)
#define LED_YELLOW_OFF      gpio_clear(YELLOW_LED)
#define LED_YELLOW_TOGGLE   gpio_toggle(YELLOW_LED)

#define LED_GREEN_ON        gpio_set(GREEN_LED)
#define LED_GREEN_OFF       gpio_clear(GREEN_LED)
#define LED_GREEN_TOGGLE    gpio_toggle(GREEN_LED)

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
