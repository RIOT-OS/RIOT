/*
 * Copyright (C) 2015 TriaGnoSys GmbH
 *               2017 Alexander Kurth, Sören Tempel, Tristan Bruns
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_bluepill
 *
 * This board can be bought very cheaply on sides like eBay or
 * AliExpress. Although the MCU nominally has 64 KiB ROM, most of them
 * have 128 KiB ROM. For more information see:
 * http://wiki.stm32duino.com/index.php?title=Blue_Pill
 *
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the bluepill board
 *
 * @author      Víctor Ariño <victor.arino@triagnosys.com>
 * @author      Sören Tempel <tempel@uni-bremen.de>
 * @author      Tristan Bruns <tbruns@uni-bremen.de>
 * @author      Alexander Kurth <kurth1@uni-bremen.de>
 */

#ifndef BOARD_H
#define BOARD_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   Macros for controlling the on-board LED.
 * @{
 */
#define LED0_PORT           GPIOC
#define LED0_PIN            GPIO_PIN(PORT_C, 13)
#define LED0_MASK           (1 << 13)

#define LED0_ON             (LED0_PORT->BSRR = (LED0_MASK << 16))
#define LED0_OFF            (LED0_PORT->BSRR = LED0_MASK)
#define LED0_TOGGLE         (LED0_PORT->ODR  ^= LED0_MASK)
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
#define XTIMER_BACKOFF      5
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
