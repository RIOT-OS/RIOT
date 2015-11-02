/*
 * Copyright (C) 2015 Sebastian Sontberg <sebastian@sontberg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    boards_xmc2go XMC 2Go
 * @ingroup     boards
 * @brief       Support for the Infineon XMC 2Go Kit with XMC1100
 * @{
 *
 * @file
 * @brief       Board specific definitions for the XMC 2Go.
 *
 * @author      Sebastian Sontberg <sebastian@sontberg.de>
 */

#ifndef BOARD_H_
#define BOARD_H_

#include "cpu.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name The nominal CPU core clock in this board
 */
#define F_CPU               (32000000UL)

/**
 * @name Peripheral timer asigned to xtimer
 * @{
 */
#define XTIMER              TIMER_DEV(0)
#define XTIMER_CHAN         (0)
/** @} */

/**
 * @name Assign the UART interface to be used for stdio
 */
#define STDIO               UART_DEV(0)
#define STDIO_BAUDRATE      (115200U)
#define STDIO_RX_BUFSIZE    (64U)

/**
 * @name LED pin definitions
 * @{
 */
#define LED_1_PIN           (PORT1_OUT_P0_Msk)
#define LED_2_PIN           (PORT1_OUT_P1_Msk)
/** @} */

/**
 * @name Macros for controlling the on-board LEDs.
 * @{
 */
#define LED_1_ON              (PORT1->OUT |= LED_1_PIN)
#define LED_1_OFF             (PORT1->OUT &= ~LED_1_PIN)
#define LED_1_TOGGLE          (PORT1->OUT ^= LED_1_PIN)

#define LED_2_ON              (PORT1->OUT |= LED_2_PIN)
#define LED_2_OFF             (PORT1->OUT &= ~LED_2_PIN)
#define LED_2_TOGGLE          (PORT1->OUT ^= LED_2_PIN)

#define LED_RED_ON            LED_1_ON
#define LED_RED_OFF           LED_1_OFF
#define LED_RED_TOGGLE        LED_1_TOGGLE
/** @} */

/**
 * @brief Initialize board cpu, LEDs and ports.
 */
void board_init(void);
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H_ */
