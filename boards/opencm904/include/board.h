/*
 * Copyright (C) 2017 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    boards_opencm904 OpenCM9.04
 * @ingroup     boards
 * @brief       Board specific files for the OpenCM9.04 board
 * @{
 *
 * @file
 * @brief       Board specific definitions for the OpenCM9.04 board
 *
 * @author      Loïc Dauphin <loic.dauphin@inria.fr>
 */

#ifndef BOARD_H
#define BOARD_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Define the location of the RIOT image in flash
 * @{
 */
#define LOCATION_VTABLE     (0x08003000)
/** @} */

/**
 * @name xtimer configuration
 * @{
 */
#define XTIMER_WIDTH        (16)
#define XTIMER_BACKOFF      5
/** @} */

/**
 * @name Macros for controlling the on-board LED.
 * @{
 */
#define LED0_PIN            GPIO_PIN(PORT_B, 9)

#define LED_PORT            GPIOB
#define LED0_MASK           (1 << 9)

#define LED0_ON             (LED_PORT->BRR = LED0_MASK)
#define LED0_OFF            (LED_PORT->BSRR  = LED0_MASK)
#define LED0_TOGGLE         (LED_PORT->ODR ^= LED0_MASK)
/** @} */

/**
 * @brief   User button
 */
#define BTN_B1_PIN          GPIO_PIN(PORT_C, 15)

/**
 * @brief Use the USART2 for STDIO on this board
 */
#define UART_STDIO_DEV      UART_DEV(0)

/**
 * @brief   Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
