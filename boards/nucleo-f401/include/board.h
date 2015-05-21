/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2015 Adam Berlinger
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    boards_nucleo-f401 Nucleo-F401RE
 * @ingroup     boards
 * @brief       Board specific files for the Nucleo F401RE evaluation board
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Nucleo F401RE evaluation evaluation board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Adam Berlinger <berlingeradam@gmail.com>
 */

#ifndef __BOARD_H
#define __BOARD_H

#include "cpu.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Define the nominal CPU core clock in this board
 */
#define F_CPU               CLOCK_CORECLOCK

/**
 * @name Assign the hardware timer
 */
#define HW_TIMER            TIMER_0

/**
 * @name Define UART device and baudrate for stdio
 * @{
 */
#define STDIO               UART_0
#define STDIO_BAUDRATE      (115200U)
#define STDIO_RX_BUFSIZE    (64U)
/** @} */

/**
 * @name LED pin definitions
 * @{
 */
#define LED_PORT            GPIOA
#define LD1_PIN             (1 << 5)
/** @} */

/**
 * @name Macros for controlling the on-board LEDs.
 * @{
 */
#define LD1_ON              (LED_PORT->BSRRL = LD1_PIN)
#define LD1_OFF             (LED_PORT->BSRRH = LD1_PIN)
#define LD1_TOGGLE          (LED_PORT->ODR ^= LD1_PIN)

/* for compatability to other boards */
#define LED_RED_ON
#define LED_RED_OFF
#define LED_RED_TOGGLE

#define LED_GREEN_ON        LD1_ON
#define LED_GREEN_OFF       LD1_OFF
#define LED_GREEN_TOGGLE    LD1_TOGGLE
/** @} */

/**
 * @brief Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /** __BOARD_H */
/** @} */
