/*
 * Copyright (C) 2015 TriaGnoSys GmbH
 *               2017 Alexander Kurth, Sören Tempel, Tristan Bruns
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_common_blxxxpill
 *
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the bluepill/blackpill board
 *
 * @author      Víctor Ariño <victor.arino@triagnosys.com>
 * @author      Sören Tempel <tempel@uni-bremen.de>
 * @author      Tristan Bruns <tbruns@uni-bremen.de>
 * @author      Alexander Kurth <kurth1@uni-bremen.de>
 */

#ifndef BOARD_COMMON_H
#define BOARD_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   Macros for controlling the on-board LED.
 * @{
 */
#ifndef LED0_PORT
#define LED0_PORT           GPIOC                                   /**< GPIO-Port the LED is connected to */
#endif
#ifndef LED0_PORTNUM
#define LED0_PORTNUM        PORT_C                                  /**< GPIO Port number the LED is connected to */
#endif
#ifndef LED0_PINNUM
#define LED0_PINNUM         (13)                                    /**< Pin number the LED is connected to */
#endif
#define LED0_PIN            GPIO_PIN(LED0_PORTNUM, LED0_PINNUM)     /**< GPIO-Pin the LED is connected to */
#define LED0_MASK           (1 << LED0_PINNUM)

#define LED0_ON             (LED0_PORT->BSRR = (LED0_MASK << 16))   /**< Turn LED0 on */
#define LED0_OFF            (LED0_PORT->BSRR = LED0_MASK)           /**< Turn LED0 off */
#define LED0_TOGGLE         (LED0_PORT->ODR  ^= LED0_MASK)          /**< Toggle LED0 */
/** @} */

/**
 * @brief   Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

/**
 * @brief   Use the 2nd UART for STDIO on this board
 */
#define STDIO_UART_DEV      UART_DEV(1)

/**
 * @name    xtimer configuration
 * @{
 */
#define XTIMER_WIDTH        (16)
#define XTIMER_BACKOFF      (19)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* BOARD_COMMON_H */
/** @} */
