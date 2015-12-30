/*
 * Copyright (C) 2015 Rakendra Thapa <rakendrathapa@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    boards_ek-lm4f120xl EK-LM4F120XL
 * @ingroup     boards
 * @brief       Board specific files for the Stellaris Launchpad LM4F120 board
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Stellaris Launchpad LM4F120 board
 *
 * @author      Rakendra Thapa <rakendrathapa@gmail.com>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "cc3200_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @name Define the boards stdio
 * @{
 */
// #define STDIO               UART_0
// #define STDIO_BAUDRATE      (115200U)
// #define STDIO_RX_BUFSIZE    (64U)
/** @} */

/**
 * @name Macros for controlling the on-board LEDs.
 * @{
 */
#define GPIO_LED_RED        9       // red
#define GPIO_LED_ORANGE     10      // orange
#define GPIO_LED_GREEN      11      // green

#define LED_RED_PIN         PIN_64
#define LED_ORANGE_PIN      PIN_01
#define LED_GREEN_PIN       PIN_02

#define PINS_IN_PORT        8

#define LED_RED_ON()        gpio_set(GPIO_LED_RED, 1)
#define LED_RED_OFF()       gpio_set(GPIO_LED_RED, 0)
#define LED_RED_TOGGLE()    /* not implemented yet */

#define LED_ORANGE_ON()     gpio_set(GPIO_LED_ORANGE, 1)
#define LED_ORANGE_OFF()    gpio_set(GPIO_LED_ORANGE, 0)
#define LED_ORANGE_TOGGLE() /* not implemented yet */

#define LED_GREEN_ON()      gpio_set(GPIO_LED_GREEN, 1)
#define LED_GREEN_OFF()     gpio_set(GPIO_LED_GREEN, 0)
#define LED_GREEN_TOGGLE()  /* not implemented yet */
/* @} */

/**
 * @brief Initialize board specific hardware, including clock, LEDs and std-IO
 */
extern void board_init(void);
extern void cpu_init(void);

#ifdef __cplusplus
}
#endif

#endif /** BOARD_H */
/** @} */
