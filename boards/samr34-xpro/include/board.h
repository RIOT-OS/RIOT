/*
 * Copyright (C) 2019 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_samr34-xpro
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
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Semtech SX1276 configuration
 * @{
 */
#define SX127X_PARAM_SPI                    SPI_DEV(0)            /**< internal SPI */
#define SX127X_PARAM_SPI_NSS                GPIO_PIN(1, 31)       /**< D10 */
#define SX127X_PARAM_RESET                  GPIO_PIN(1, 15)       /**< A0  */
#define SX127X_PARAM_DIO0                   GPIO_PIN(1, 16)       /**< D2  */
#define SX127X_PARAM_DIO1                   GPIO_PIN(0, 11)       /**< D3  */
#define SX127X_PARAM_DIO2                   GPIO_PIN(0, 12)       /**< D4  */
#define SX127X_PARAM_DIO3                   GPIO_PIN(1, 17)       /**< D5  */
#define SX127X_PARAM_PASELECT               SX127X_PA_RFO         /**< no boost */
/** @}*/

/**
 * @name Board specific configuration
 *  @{
 */
#define TCXO_PWR_PIN                        GPIO_PIN(PA, 9)     /**< 32 MHz oscillator for radio enable */
#define TX_OUTPUT_SEL_PIN                   GPIO_PIN(PA, 13)    /**< BAND_SEL */
/** @}*/

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#define LED_PORT                    PORT->Group[0]                      /**< GPIO port      */

#define LED0_PIN                    GPIO_PIN(PA, 18)                    /**< GPIO pin       */
#define LED0_MASK                   (1 << 18)                           /**< GPIO pin mask  */
#define LED0_ON                     (LED_PORT.OUTCLR.reg = LED0_MASK)   /**< enable LED     */
#define LED0_OFF                    (LED_PORT.OUTSET.reg = LED0_MASK)   /**< disable LED    */
#define LED0_TOGGLE                 (LED_PORT.OUTTGL.reg = LED0_MASK)   /**< toggle LED     */

#define LED1_PIN                    GPIO_PIN(PA, 19)                    /**< GPIO pin       */
#define LED1_MASK                   (1 << 19)                           /**< GPIO pin mask  */
#define LED1_ON                     (LED_PORT.OUTCLR.reg = LED1_MASK)   /**< enable LED     */
#define LED1_OFF                    (LED_PORT.OUTSET.reg = LED1_MASK)   /**< disable LED    */
#define LED1_TOGGLE                 (LED_PORT.OUTTGL.reg = LED1_MASK)   /**< toggle LED     */
/** @} */

/**
 * @name    BTN0 (SW0 Button) pin definitions
 * @{
 */
#define BTN0_PIN                    GPIO_PIN(PA, 28)                    /**< GPIO pin       */
#define BTN0_MODE                   GPIO_IN_PU                          /**< Pull Up GPIO   */
/** @} */

/**
 * @name MTD configuration
 * @{
 */
#define MTD_0 mtd_dev_get(0)                  /**< First memory type device */
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* BOARD_H */
