/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    boards_weio WeIO development kit
 * @ingroup     boards
 * @brief       Support for the WeIO board
 * @{
 *
 * @file
 * @brief       Board specific definitions for the WeIO board
 *
 * @author      Paul RATHGEB <paul.rathgeb@skynet.be>
 */

#ifndef BOARD_H_
#define BOARD_H_

#include <stdint.h>

#include "bitarithm.h"
#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The nominal CPU core clock in this board
 */
#define F_CPU               (48000000)

/**
 * @name Assign the UART interface to be used for stdio
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
#define LED_PORT            LPC_GPIO
#define LED1_PIN            BIT13
#define LED2_PIN            BIT14
#define LED3_PIN            BIT15
/** @} */

/**
 * @name Macros for controlling the on-board LEDs.
 * @{
 */
#define LED1_ON              (LED_PORT->CLR[1] = LED1_PIN)
#define LED1_OFF             (LED_PORT->SET[1] = LED1_PIN)
#define LED1_TOGGLE          (LED_PORT->NOT[1] = LED1_PIN)
#define LED2_ON              (LED_PORT->CLR[1] = LED2_PIN)
#define LED2_OFF             (LED_PORT->SET[1] = LED2_PIN)
#define LED2_TOGGLE          (LED_PORT->NOT[1] = LED2_PIN)
#define LED3_ON              (LED_PORT->CLR[1] = LED3_PIN)
#define LED3_OFF             (LED_PORT->SET[1] = LED3_PIN)
#define LED3_TOGGLE          (LED_PORT->NOT[1] = LED3_PIN)
/* for compatibility to other boards */
#define LED_GREEN_ON        LED2_ON
#define LED_GREEN_OFF       LED2_OFF
#define LED_GREEN_TOGGLE    LED2_TOGGLE
#define LED_RED_ON          LED1_ON
#define LED_RED_OFF         LED1_OFF
#define LED_RED_TOGGLE      LED1_TOGGLE
/** @} */

/**
 * @brief Initialize board specific hardware, include clocks, LEDs and stdio
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H_ */
/** @} */
