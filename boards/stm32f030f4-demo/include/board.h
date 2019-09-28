/*
 * Copyright (C) 2019 Benjamin Valentin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_stm32f030f4-demo
 *
 * This board can be bought very cheaply (< 1€) on sites like eBay or
 * AliExpress.
 *
 * @brief       Support for the STM32F030F4 Demo Board
 * @{
 *
 * @file
 * @brief       Pin definitions and board configuration options
 *
 * @author      Benjamin Valentin <benpicco@googlemail.com>
 */

#ifndef BOARD_H
#define BOARD_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Xtimer configuration
 * @{
 */
#define XTIMER_WIDTH                (16)
/** @} */

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#define LED0_PORT           GPIOA
#define LED0_PIN            GPIO_PIN(PORT_A, 4)
#define LED0_MASK           (1 << 4)

#define LED0_ON             (LED0_PORT->BSRR = (LED0_MASK << 16))
#define LED0_OFF            (LED0_PORT->BSRR = (LED0_MASK <<  0))
#define LED0_TOGGLE         (LED0_PORT->ODR  ^= LED0_MASK)
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
