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

#define DEBUG_PIN 10 // P1.10 on launchpad (bottom pin on left side)

/**
 * Define the nominal CPU core clock in this board
 */
#define F_CPU               80000000

/**
 * Assign the hardware timer
 */
#define HW_TIMER            TIMER_3
#define HWTIMER_WAIT_OVERHEAD (1430)
#define HWTIMER_SPIN_BARRIER HWTIMER_WAIT_OVERHEAD

#define TIMER_NUM_CHANNELS 4

#define XTIMER  TIMER_0
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
