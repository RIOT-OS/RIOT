/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    boards_nrf51dk NRF51-DK
 * @ingroup     boards
 * @brief       Board specific files for the NRF51-DK board
 * @{
 *
 * @file
 * @brief       Board specific implementations for the NRF51-DK board
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
#define XTIMER_SHIFT_ON_COMPARE     (2)
#define XTIMER_BACKOFF              (40)
/** @} */

/**
 * @name    LED pin definitions
 * @{
 */
#define LED1_PIN            (1 << 21)
#define LED2_PIN            (1 << 22)
#define LED3_PIN            (1 << 23)
#define LED4_PIN            (1 << 24)
/** @} */

/**
 * @name    Macros for controlling the on-board LEDs
 * @{
 */
#define LED1_ON          (NRF_GPIO->OUTCLR = LED1_PIN)
#define LED1_OFF         (NRF_GPIO->OUTSET = LED1_PIN)
#define LED1_TOGGLE      (NRF_GPIO->OUT   ^= LED1_PIN)
#define LED2_ON          (NRF_GPIO->OUTCLR = LED2_PIN)
#define LED2_OFF         (NRF_GPIO->OUTSET = LED2_PIN)
#define LED2_TOGGLE      (NRF_GPIO->OUT   ^= LED2_PIN)
#define LED3_ON          (NRF_GPIO->OUTCLR = LED3_PIN)
#define LED3_OFF         (NRF_GPIO->OUTSET = LED3_PIN)
#define LED3_TOGGLE      (NRF_GPIO->OUT   ^= LED3_PIN)
#define LED4_ON          (NRF_GPIO->OUTCLR = LED4_PIN)
#define LED4_OFF         (NRF_GPIO->OUTSET = LED4_PIN)
#define LED4_TOGGLE      (NRF_GPIO->OUT   ^= LED4_PIN)
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
