/*
 * Copyright (C) 2022 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_common_stm32
 * @brief       Common LED macros
 * @{
 *
 * @file
 * @brief       Common LED macros
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * This idea is that STM32 boards only define the pin and port number of LEDs
 * and this header provides the rest of the defines
 */

#ifndef STM32_LEDS_H
#define STM32_LEDS_H

/* GPIO_PORT() macro. This define even works when GPIO LL is not in used */
#include "gpio_ll_arch.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Common LED pin definitions for STM32 boards
 * @{
 */
#if defined(LED0_PORT_NUM) && defined (LED0_PIN_NUM)
#  define LED0_PORT           ((GPIO_TypeDef *)GPIO_PORT(LED0_PORT_NUM))
#  define LED0_PIN            GPIO_PIN(LED0_PORT_NUM, LED0_PIN_NUM)
#  define LED0_MASK           (1 << LED0_PIN_NUM)
#  define LED0_ON             (LED0_PORT->BSRR = LED0_MASK)
#  define LED0_OFF            (LED0_PORT->BSRR = (LED0_MASK << 16))
#  define LED0_TOGGLE         (LED0_PORT->ODR  ^= LED0_MASK)
#endif

#if defined(LED1_PORT_NUM) && defined (LED1_PIN_NUM)
#  define LED1_PORT           ((GPIO_TypeDef *)GPIO_PORT(LED1_PORT_NUM))
#  define LED1_PIN            GPIO_PIN(LED1_PORT_NUM, LED1_PIN_NUM)
#  define LED1_MASK           (1 << LED1_PIN_NUM)
#  define LED1_ON             (LED1_PORT->BSRR = LED1_MASK)
#  define LED1_OFF            (LED1_PORT->BSRR = (LED1_MASK << 16))
#  define LED1_TOGGLE         (LED1_PORT->ODR  ^= LED1_MASK)
#endif

#if defined(LED2_PORT_NUM) && defined (LED2_PIN_NUM)
#  define LED2_PORT           ((GPIO_TypeDef *)GPIO_PORT(LED2_PORT_NUM))
#  define LED2_PIN            GPIO_PIN(LED2_PORT_NUM, LED2_PIN_NUM)
#  define LED2_MASK           (1 << LED2_PIN_NUM)
#  define LED2_ON             (LED2_PORT->BSRR = LED2_MASK)
#  define LED2_OFF            (LED2_PORT->BSRR = (LED2_MASK << 16))
#  define LED2_TOGGLE         (LED2_PORT->ODR  ^= LED2_MASK)
#endif

#if defined(LED3_PORT_NUM) && defined (LED3_PIN_NUM)
#  define LED3_PORT           ((GPIO_TypeDef *)GPIO_PORT(LED3_PORT_NUM))
#  define LED3_PIN            GPIO_PIN(LED3_PORT_NUM, LED3_PIN_NUM)
#  define LED3_MASK           (1 << LED3_PIN_NUM)
#  define LED3_ON             (LED3_PORT->BSRR = LED3_MASK)
#  define LED3_OFF            (LED3_PORT->BSRR = (LED3_MASK << 16))
#  define LED3_TOGGLE         (LED3_PORT->ODR  ^= LED3_MASK)
#endif

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* STM32_LEDS_H */
/** @} */
