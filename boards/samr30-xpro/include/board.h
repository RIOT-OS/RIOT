/*
 * Copyright (C) 2017 Baptiste Clenet <bapclenet@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_samr30-xpro Atmel SAM R30 Xplained Pro
 * @ingroup     boards
 * @brief       Support for the Atmel SAM R30 Xplained Pro board.
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Atmel SAM R30 Xplained Pro board.
 *
 * @author      Baptiste Clenet <bapclenet@gmail.com>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name NG_AT86RF212B configuration
 * @{
 */
#define AT86RF2XX_PARAM_SPI         SPI_DEV(0)
#define AT86RF2XX_PARAM_CS          GPIO_PIN(PB, 31)
#define AT86RF2XX_PARAM_INT         GPIO_PIN(PB, 0)
#define AT86RF2XX_PARAM_SLEEP       GPIO_PIN(PA, 20)
#define AT86RF2XX_PARAM_RESET       GPIO_PIN(PB, 15)
#define AT86RF2XX_PARAM_SPI_CLK     SPI_CLK_5MHZ
/** @}*/

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#define LED_PORT                    PORT->Group[0]

#define LED0_PIN                    GPIO_PIN(PA, 18)
#define LED0_MASK                   (1 << 18)
#define LED0_ON                     (LED_PORT.OUTCLR.reg = LED0_MASK)
#define LED0_OFF                    (LED_PORT.OUTSET.reg = LED0_MASK)
#define LED0_TOGGLE                 (LED_PORT.OUTTGL.reg = LED0_MASK)

#define LED1_PIN                    GPIO_PIN(PA, 19)
#define LED1_MASK                   (1 << 19)
#define LED1_ON                     (LED_PORT.OUTCLR.reg = LED1_MASK)
#define LED1_OFF                    (LED_PORT.OUTSET.reg = LED1_MASK)
#define LED1_TOGGLE                 (LED_PORT.OUTTGL.reg = LED1_MASK)
/** @} */

/**
 * @name    BTN0 (SW0 Button) pin definitions
 * @{
 */
#define BTN0_PIN                    GPIO_PIN(PA, 28)
#define BTN0_MODE                   GPIO_IN_PU
/** @} */

/**
 * @name    Antenna configuration pin interface
 * @{
 */
#define RFCTL1_PIN                  GPIO_PIN(PA, 9)
#define RFCTL2_PIN                  GPIO_PIN(PA, 12)
/** @} */

/**
 * @brief   Antenna configuration values
 */
enum {
    RFCTL_ANTENNA_BOARD,
    RFCTL_ANTENNA_EXT,
};

/**
 * @name    Default antenna configuration
 * @{
 */
#ifndef RFCTL_ANTENNA_DEFAULT
#define RFCTL_ANTENNA_DEFAULT      RFCTL_ANTENNA_BOARD
#endif
/** @} */

/**
 * @brief   Set antenna switch
 */
void board_antenna_config(uint8_t antenna);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
