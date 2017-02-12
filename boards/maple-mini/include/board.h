/*
 * Copyright (C) 2016 Frits Kuipers
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    boards_maple-mini maple-mini
 * @ingroup     boards
 * @brief       Board specific files for the maple-mini board
 * @{
 *
 * @file
 * @brief       Board specific definitions for the maple-mini board
 *
 * @author      Frits Kuipers <frits.kuipers@gmail.com>
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
#define XTIMER_BACKOFF      5
/** @} */

/**
 * @name Macros for controlling the on-board LEDs.
 * @{
 */
#define LED0_PIN            GPIO_PIN(PORT_B, 1)

#define LED_PORT            GPIOB
#define LED0_MASK           (1 << 1)

#define LED0_ON             (LED_PORT->BSRR = LED0_MASK)
#define LED0_OFF            (LED_PORT->BRR  = LED0_MASK)
#define LED0_TOGGLE         (LED_PORT->ODR ^= LED0_MASK)
/** @} */

/**
 * @brief   User button
 */
#define BTN_B1_PIN          GPIO_PIN(PORT_B, 8)

/**
 * @brief Use the USART1 for STDIO on this board
 */
#define UART_STDIO_DEV      UART_DEV(1)

/**
 * @brief   Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
