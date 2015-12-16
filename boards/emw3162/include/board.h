/*
 * Copyright (C) 2015 Engineering-Spirit
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    boards_emw3162 EMW3162
 * @ingroup     boards
 * @brief       Board specific files for the EMW3162 board
 * @{
 *
 * @file
 * @brief       Board specific definitions for the EMW3162 evaluation board
 *
 * @author      Nick v. IJzendoorn <nijzendoorn@engineering-spirit.nl>
 */

#ifndef BOARD_H_
#define BOARD_H_

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
 * @name xtimer configuration
 * @{
 */
#define XTIMER              TIMER_0
#define XTIMER_CHAN         (0)
#define XTIMER_OVERHEAD     (6)
#define XTIMER_BACKOFF      (5)
/** @} */

/**
 * @name Define UART device and baudrate for stdio
 * @{
 */
#define STDIO               UART_DEV(0)
#define STDIO_BAUDRATE      (115200U)
#define STDIO_RX_BUFSIZE    (64U)
/** @} */

/**
 * @name LED pin definitions
 * @{
 */
#define LED_PORT            GPIOB
#define D1_PIN              (1 << 0)
#define D2_PIN              (1 << 1)
/** @} */

/**
 * @name Macros for controlling the on-board LEDs.
 * @{
 */
#define D1_ON               (LED_PORT->BSRRL = D1_PIN)
#define D1_OFF              (LED_PORT->BSRRH = D1_PIN)
#define D1_TOGGLE           (LED_PORT->ODR ^= D1_PIN)
#define D2_ON               (LED_PORT->BSRRL = D2_PIN)
#define D2_OFF              (LED_PORT->BSRRH = D2_PIN)
#define D2_TOGGLE           (LED_PORT->ODR ^= D2_PIN)

/* for compatability to other boards */
#define LED_GREEN_ON        D1_ON
#define LED_GREEN_OFF       D1_OFF
#define LED_GREEN_TOGGLE    D1_TOGGLE
#define LED_RED_ON          D2_ON
#define LED_RED_OFF         D2_OFF
#define LED_RED_TOGGLE      D2_TOGGLE
/** @} */

/**
 * @brief Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H_ */
/** @} */
