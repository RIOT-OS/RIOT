/*
 * Copyright (C) 2017 Inria
 *               2017 OTAKeys
 *               2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_common_nucleo144 STM32 Nucleo-144
 * @ingroup     boards
 * @brief       Support for STM32 Nucleo-144 boards
 * @{
 *
 * @file
 * @brief       Common pin definitions and board configuration options
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @author      Sebastian Meiling <s@mlng.net>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef BOARD_H
#define BOARD_H

#include "board_nucleo.h"
#include "arduino_pinmap.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#if defined(CPU_MODEL_STM32L496ZG) || defined(CPU_MODEL_STM32L4R5ZI) || \
    defined(CPU_MODEL_STM32L552ZE)
#define LED0_PORT           GPIOC
#define LED0_PIN            GPIO_PIN(PORT_C, 7)
#define LED0_MASK           (1 << 7)
#else
#define LED0_PORT           GPIOB
#define LED0_PIN            GPIO_PIN(PORT_B, 0)
#define LED0_MASK           (1 << 0)
#endif
#define LED0_ON             (LED0_PORT->BSRR = LED0_MASK)
#define LED0_OFF            (LED0_PORT->BSRR = (LED0_MASK << 16))
#define LED0_TOGGLE         (LED0_PORT->ODR  ^= LED0_MASK)

#define LED1_PIN            GPIO_PIN(PORT_B, 7)
#define LED1_MASK           (1 << 7)
#define LED1_ON             (GPIOB->BSRR = LED1_MASK)
#define LED1_OFF            (GPIOB->BSRR = (LED1_MASK << 16))
#define LED1_TOGGLE         (GPIOB->ODR  ^= LED1_MASK)

#if defined(CPU_MODEL_STM32L552ZE)
#define LED2_PORT           GPIOA
#define LED2_PIN            GPIO_PIN(PORT_A, 9)
#define LED2_MASK           (1 << 9)
#else
#define LED2_PORT           GPIOB
#define LED2_PIN            GPIO_PIN(PORT_B, 14)
#define LED2_MASK           (1 << 14)
#endif
#define LED2_ON             (LED2_PORT->BSRR = LED2_MASK)
#define LED2_OFF            (LED2_PORT->BSRR = (LED2_MASK << 16))
#define LED2_TOGGLE         (LED2_PORT->ODR  ^= LED2_MASK)

/* the Nucleo144 boards always use LED0, as there is no dual use of its pin */
#ifndef AUTO_INIT_LED0
#define AUTO_INIT_LED0
#endif
/** @} */

/**
 * @name    User button
 * @{
 */
#define BTN0_PIN            GPIO_PIN(PORT_C, 13)
#define BTN0_MODE           GPIO_IN_PD
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
