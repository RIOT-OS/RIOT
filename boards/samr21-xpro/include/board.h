/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_samr21-xpro Atmel SAM R21 Xplained Pro
 * @ingroup     boards
 * @brief       Support for the Atmel SAM R21 Xplained Pro board.
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Atmel SAM R21 Xplained Pro
 *              board
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 */

#ifndef BOARD_H_
#define BOARD_H_

#include "cpu.h"
#include "periph_conf.h"
#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Define the nominal CPU core clock in this board
 */
#define F_CPU               (CLOCK_CORECLOCK)

/**
 * Assign the hardware timer
 */
#define HW_TIMER            TIMER_1

/**
* @name AT86RF233 configuration
* @{
*/
#define AT86RF233_SPI        (SPI_0)
#define AT86RF233_CS         GPIO(PB, 31)
#define AT86RF233_INT        GPIO(PB, 0)
#define AT86RF233_RESET      GPIO(PB, 15)
#define AT86RF233_SLEEP      GPIO(PA, 20)
#define AT86RF233_SPI_CLK    (SPI_SPEED_1MHZ)
/** @}*/

/**
* @name AT86RF231 config
* @{
*/
#define AT86RF231_SPI      SPI_0
#define AT86RF231_CS       GPIO(PB, 31)
#define AT86RF231_INT      GPIO(PB, 0)
#define AT86RF231_RESET    GPIO(PB, 15)
#define AT86RF231_SLEEP    GPIO(PA, 20)

#define AT86RF231_SPI_SPEED SPI_SPEED_1MHZ
/** @} */

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
#define LED_PORT            PORT->Group[0]
#define LED_PIN             (19)
/** @} */

/**
 * @name Macros for controlling the on-board LEDs.
 * @{
 */
#define LED_ON              (LED_PORT.OUTCLR.reg = (1 << LED_PIN))
#define LED_OFF             (LED_PORT.OUTSET.reg = (1 << LED_PIN))
#define LED_TOGGLE          (LED_PORT.OUTTGL.reg = (1 << LED_PIN))

/* for compatability to other boards */
#define LED_GREEN_ON        /* not available */
#define LED_GREEN_OFF       /* not available */
#define LED_GREEN_TOGGLE    /* not available */
#define LED_ORANGE_ON       /* not available */
#define LED_ORANGE_OFF      /* not available */
#define LED_ORANGE_TOGGLE   /* not available */
#define LED_RED_ON          LED_ON
#define LED_RED_OFF         LED_OFF
#define LED_RED_TOGGLE      LED_TOGGLE
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
