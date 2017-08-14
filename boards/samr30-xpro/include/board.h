/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_saml21-xpro Atmel SAM R30 Xplained Pro
 * @ingroup     boards
 * @brief       Support for the Atmel SAM R30 Xplained Pro board.
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Atmel SAM R30 Xplained Pro board.
 *
 * @author      Pierre Godicheau <pierre.godicheau@gmail.com>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif




/**
* @name NG_AT86RF212B configuration
 *
 * {spi bus, spi speed, cs pin, int pin, reset pin, sleep pin}
 */
#define AT86RF2XX_PARAMS_BOARD      {.spi = SPI_DEV(0), \
                                     .spi_clk = SPI_CLK_1MHZ, \
                                     .cs_pin = GPIO_PIN(PB, 31), \
                                     .int_pin = GPIO_PIN(PB, 0), \
                                     .sleep_pin = GPIO_PIN(PA, 20), \
                                     .reset_pin = GPIO_PIN(PB, 15)}
/** @}*//*
 * @name    LED pin definitions and handlers
 * @{
 */
#define LED_PORT                    PORT->Group[0]

#define LED0GREEN_PIN               GPIO_PIN(0, 18)
#define LED0GREEN_MASK              (1 << 18)
#define LED0GREEN_ON                (LED_PORT.OUTCLR.reg = LED0GREEN_MASK)
#define LED0GREEN_OFF               (LED_PORT.OUTSET.reg = LED0GREEN_MASK)
#define LED0GREEN_TOGGLE            (LED_PORT.OUTTGL.reg = LED0GREEN_MASK)

#define LED1ORANGE_PIN              GPIO_PIN(0, 19)
#define LED1ORANGE_MASK             (1 << 19)
#define LED1ORANGE_ON               (LED_PORT.OUTCLR.reg = LED1ORANGE_MASK)
#define LED1ORANGE_OFF              (LED_PORT.OUTSET.reg = LED1ORANGE_MASK)
#define LED1ORANGE_TOGGLE           (LED_PORT.OUTTGL.reg = LED1ORANGE_MASK)
/** @} */

/**
 * @name QT BTN1 (Button) pin definitions
 * @{
 */
#define BTN0_PORT                   PORT->Group[PA]
#define BTN0_PIN                    GPIO_PIN(PA, 6)
#define BTN0_MODE                   GPIO_IN_PU
/** @} */


/**
 * @brief   Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
