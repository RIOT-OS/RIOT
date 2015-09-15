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
#include "periph/uart.h"
#include "periph/timer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Define the boards stdio
 * @{
 */
#define STDIO               UART_0
#define STDIO_BAUDRATE      (115200U)
#define STDIO_RX_BUFSIZE    (64U)
/** @} */

/**
 * @name Assign the hardware timer
 */
#define HW_TIMER            TIMER_0

/**
 * @name Macros for controlling the on-board LEDs.
 * @{
 */
#define LED_GREEN_PIN       0x08
#define LED_BLUE_PIN        0x04
#define LED_RED_PIN         0x02

#define LED_GREEN_ON        GPIO_PORTF_DATA_R |= LED_GREEN_PIN
#define LED_GREEN_OFF       GPIO_PORTF_DATA_R &= ~(LED_GREEN_PIN)
#define LED_GREEN_TOGGLE    /* not available */

#define LED_BLUE_ON         GPIO_PORTF_DATA_R |= LED_BLUE_PIN
#define LED_BLUE_OFF        GPIO_PORTF_DATA_R &= ~(LED_BLUE_PIN)
#define LED_BLUE_TOGGLE     /* not available */

#define LED_RED_ON          GPIO_PORTF_DATA_R |= LED_RED_PIN
#define LED_RED_OFF         GPIO_PORTF_DATA_R &= ~(LED_RED_PIN)
#define LED_RED_TOGGLE      /* not available */
/* @} */

/**
 * @brief Initialize board specific hardware, including clock, LEDs and std-IO
 */
extern void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /** BOARD_H */
/** @} */
