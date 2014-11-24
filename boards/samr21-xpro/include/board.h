/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    board_atsamr21-xpro Atmel SAM R21 Xplained Pro
 * @ingroup     boards
 * @brief       Support for the Atmel SAM R21 Xplained Pro board.
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Atmel SAM R21 Xplained Pro board.
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 */

#ifndef __BOARD_H
#define __BOARD_H

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Define the nominal CPU core clock in this board
 */
#define F_CPU               (8000000UL)

/**
 * Assign the hardware timer
 */
#define HW_TIMER            TIMER_1

/**
* @name AT86RF231 config
* @{ 
*/
#define AT86RF231_SPI      SPI_0
#define AT86RF231_CS       GPIO_4
#define AT86RF231_INT      GPIO_5
#define AT86RF231_RESET    GPIO_6
#define AT86RF231_SLEEP    GPIO_7

#define AT86RF231_SPI_SPEED SPI_SPEED_1MHZ
/** @}*/
/**
 * @name Define UART device and baudrate for stdio
 * @{
 */
#define STDIO               UART_1
#define STDIO_BAUDRATE      (9600U)
#define STDIO_BUFSIZE		(64U)
/** @} */

/**
 * @name LED pin definitions
 * @{
 */
#define LED_PORT            PORT->Group[0]
#define LED_PIN             (19)
/** @} */

/**
 * @name Macros for controlling the on-board LEDs.
 * @{
 */
#define LED_ON              (LED_PORT.OUTCLR.reg = LED_PIN)//ON = LOW
#define LED_OFF             (LED_PORT.OUTSET.reg = LED_PIN)
#define LED_TOGGLE          (LED_PORT.OUTTGL.reg = LED_PIN)

/* for compatability to other boards */
#define LED_GREEN_ON        /* not available */
#define LED_GREEN_OFF       /* not available */
#define LED_GREEN_TOGGLE    /* not available */
#define LED_ORANGE_ON        /* not available */
#define LED_ORANGE_OFF       /* not available */
#define LED_ORANGE_TOGGLE    /* not available */
#define LED_RED_ON          LED_ON
#define LED_RED_OFF         LED_OFF
#define LED_RED_TOGGLE      LED_TOGGLE
/** @} */

/**
 * @brief Define the type for the radio packet length for the transceiver
 */
typedef uint8_t radio_packet_length_t;

/**
 * @brief Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /** __BOARD_H */
/** @} */
