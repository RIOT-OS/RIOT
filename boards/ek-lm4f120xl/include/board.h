/*
 * Copyright (C) 2015 Rakendra Thapa <rakendrathapa@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    boards_ek-lm4f120xl EK-LM4F120XL
 * @ingroup     boards
 * @brief       Board specific files for the Stellaris Launchpad LM4F120 board
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Stellaris Launchpad LM4F120 board
 *
 * @author      Rakendra Thapa <rakendrathapa@gmail.com>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "periph/uart.h"
#include "periph/timer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   LED pin definitions and handlers
 * @{
 */
#define LED0_PIN            GPIO_PIN(5, 7)
#define LED1_PIN            GPIO_PIN(5, 2)
#define LED2_PIN            GPIO_PIN(5, 1)

#define LED_PORT            (GPIO_PORTF_DATA_R)
#define LED0_MASK           (1 << 7)
#define LED1_MASK           (1 << 2)
#define LED2_MASK           (1 << 1)

#define LED0_ON             (LED_PORT |=  LED0_MASK)
#define LED0_OFF            (LED_PORT &= ~LED0_MASK)
#define LED0_TOGGLE         (LED_PORT ^=  LED0_MASK)

#define LED1_ON             (LED_PORT |=  LED1_MASK)
#define LED1_OFF            (LED_PORT &= ~LED1_MASK)
#define LED1_TOGGLE         (LED_PORT ^=  LED1_MASK)

#define LED2_ON             (LED_PORT |=  LED2_MASK)
#define LED2_OFF            (LED_PORT &= ~LED2_MASK)
#define LED2_TOGGLE         (LED_PORT ^=  LED2_MASK)
/* @} */

/**
 * @brief Initialize board specific hardware, including clock, LEDs and std-IO
 */
extern void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /** BOARD_H */
/** @} */
