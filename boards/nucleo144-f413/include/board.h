/*
 * Copyright (C) 2016 Inria
 *               2017 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    boards_nucleo144-f413 Nucleo-F413
 * @ingroup     boards
 * @brief       Board specific files for the nucleo144-f413 board
 * @{
 *
 * @file
 * @brief       Board specific definitions for the nucleo144-f413 board
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @author      Vincent Dupont <vincent@otakeys.com>
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
#define XTIMER_DEV          TIMER_DEV(0)
#define XTIMER_CHAN         (0)
#define XTIMER_OVERHEAD     (6)
#define XTIMER_BACKOFF      (5)
/** @} */

/**
 * @brief   LED pin definitions and handlers
 * @{
 */
#define LED_CREG            BSRRH
#define LED_SREG            BSRRL

#define LED0_PIN            GPIO_PIN(PORT_B, 0)
#define LED0_MASK           (1 << 0)
#define LED0_ON             (GPIOA->LED_SREG = LED0_MASK)
#define LED0_OFF            (GPIOA->LED_CREG = LED0_MASK)
#define LED0_TOGGLE         (GPIOA->ODR     ^= LED0_MASK)

#define LED1_PIN            GPIO_PIN(PORT_B, 7)
#define LED1_MASK           (1 << 7)
#define LED1_OFF            (GPIOA->LED_CREG = LED1_MASK)
#define LED1_ON             (GPIOA->LED_SREG = LED1_MASK)
#define LED1_TOGGLE         (GPIOA->ODR     ^= LED1_MASK)

#define LED2_PIN            GPIO_PIN(PORT_B, 14)
#define LED2_MASK           (1 << 14)
#define LED2_ON             (GPIOA->LED_SREG = LED2_MASK)
#define LED2_OFF            (GPIOA->LED_CREG = LED2_MASK)
#define LED2_TOGGLE         (GPIOA->ODR     ^= LED2_MASK)
/** @} */

/**
 * @brief   User button
 */
#define BTN_B1_PIN          GPIO_PIN(PORT_C, 13)

/**
 * @brief   Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
