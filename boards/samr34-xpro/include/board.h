/*
 * Copyright (C) 2019 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_samr34-xpro Microchip SAM R34 Xplained Pro
 * @ingroup     boards
 * @brief       Support for the Microchip SAM R34 Xplained Pro board.
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Microchip SAM R34
 *              Xplained Pro board.
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Semtech SX1276 configuration
 * @{
 */
#define SX127X_PARAM_SPI                    (SPI_DEV(0))
#define SX127X_PARAM_SPI_NSS                GPIO_PIN(1, 31)       /* D10 */
#define SX127X_PARAM_RESET                  GPIO_PIN(1, 15)       /* A0 */
#define SX127X_PARAM_DIO0                   GPIO_PIN(1, 16)      /* D2 */
#define SX127X_PARAM_DIO1                   GPIO_PIN(0, 11)       /* D3 */
#define SX127X_PARAM_DIO2                   GPIO_PIN(0, 12)       /* D4 */
#define SX127X_PARAM_DIO3                   GPIO_PIN(1, 17)       /* D5 */
#define SX127X_PARAM_PASELECT               (SX127X_PA_RFO)
/** @}*/

/**
 * @name Board specific configuration
 *  @{
 */
#define TCXO_PWR_PIN                        GPIO_PIN(PA, 9)
#define TX_OUTPUT_SEL_PIN                   GPIO_PIN(PA, 13)
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
#define BTN0_PORT                   PORT->Group[0]
#define BTN0_PIN                    GPIO_PIN(PA, 28)
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
