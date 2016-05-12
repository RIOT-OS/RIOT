/*
 * Copyright (C) 2016 Stephane D'Alu
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    boards_ble400 BLE400
 * @ingroup     boards
 * @brief       Board specific files for the BLE400 board
 * @{
 *
 * @file
 * @brief       Board specific definitions for the BLE400 board
 *
 * @author      Stephane D'Alu <sdalu@sdalu.com>
 */

#ifndef BOARD_H_
#define BOARD_H_

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Xtimer configuration
 * @{
 */
#define XTIMER                      (0)
#define XTIMER_CHAN                 (0)
#define XTIMER_MASK                 (0xff000000)
#define XTIMER_BACKOFF              (40)
/** @} */

/**
 * @brief   LED pin configuration
 * @{
 */
#define LED0_PIN            GPIO_PIN(0, 18)
#define LED1_PIN            GPIO_PIN(0, 19)
#define LED2_PIN            GPIO_PIN(0, 20)
#define LED3_PIN            GPIO_PIN(0, 21)
#define LED4_PIN            GPIO_PIN(0, 22)

#define LED0_MASK           (1 << 18)
#define LED1_MASK           (1 << 19)
#define LED2_MASK           (1 << 20)
#define LED3_MASK           (1 << 21)
#define LED4_MASK           (1 << 22)

#define LED0_ON             (NRF_GPIO->OUTSET = LED0_MASK)
#define LED0_OFF            (NRF_GPIO->OUTCLR = LED0_MASK)
#define LED0_TOGGLE         (NRF_GPIO->OUT   ^= LED0_MASK)

#define LED1_ON             (NRF_GPIO->OUTSET = LED1_MASK)
#define LED1_OFF            (NRF_GPIO->OUTCLR = LED1_MASK)
#define LED1_TOGGLE         (NRF_GPIO->OUT   ^= LED1_MASK)

#define LED2_ON             (NRF_GPIO->OUTSET = LED2_MASK)
#define LED2_OFF            (NRF_GPIO->OUTCLR = LED2_MASK)
#define LED2_TOGGLE         (NRF_GPIO->OUT   ^= LED2_MASK)

#define LED3_ON             (NRF_GPIO->OUTSET = LED3_MASK)
#define LED3_OFF            (NRF_GPIO->OUTCLR = LED3_MASK)
#define LED3_TOGGLE         (NRF_GPIO->OUT   ^= LED3_MASK)

#define LED4_ON             (NRF_GPIO->OUTSET = LED4_MASK)
#define LED4_OFF            (NRF_GPIO->OUTCLR = LED4_MASK)
#define LED4_TOGGLE         (NRF_GPIO->OUT   ^= LED4_MASK)
/** @} */

/**
 * @brief   Button pin configuration
 * @{
 */
#define BUTTON1_PIN         (GPIO_PIN(0, 16))
#define BUTTON2_PIN         (GPIO_PIN(0, 17))
/** @} */

/**
 * @brief Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H_ */
/** @} */
