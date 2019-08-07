/*
 * Copyright (C) 2017 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_opencm904
 * @{
 *
 * @file
 * @brief       Board specific definitions for the OpenCM9.04 board
 *
 * @author      Loïc Dauphin <loic.dauphin@inria.fr>
 * @author      Sebastian Meiling <s@mlng.net>
 */

#ifndef BOARD_H
#define BOARD_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name xtimer configuration
 * @{
 */
#define XTIMER_WIDTH        (16)
#define XTIMER_BACKOFF      (19)
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
 * @name User button
 * @{
 */
#define BTN0_PIN            GPIO_PIN(PORT_C, 15)
#define BTN0_MODE           GPIO_IN
/** @} */

/**
 * @brief Pin used to switch RX and TX mode for the Dynamixel TTL bus
 *
 * set   = TX mode
 * clear = RX mode
 *
 */
#define DXL_DIR_PIN         GPIO_PIN(PORT_B, 5)

/**
 * @brief Use the USART2 for STDIO on this board
 */
#define STDIO_UART_DEV      UART_DEV(0)

/**
 * @name Override with ROBOTIS Bluetooth antenna baudrate for STDIO
 * @{
 */
#ifndef STDIO_UART_BAUDRATE
#define STDIO_UART_BAUDRATE (921600UL)
#endif
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
