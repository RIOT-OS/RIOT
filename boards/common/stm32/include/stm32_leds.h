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
#include "kernel_defines.h"

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
#  if IS_ACTIVE(LED0_IS_INVERTED)
#    define LED0_ON             (LED0_PORT->BSRR = (LED0_MASK << 16))
#    define LED0_OFF            (LED0_PORT->BSRR = LED0_MASK)
#  else
#    define LED0_ON             (LED0_PORT->BSRR = LED0_MASK)
#    define LED0_OFF            (LED0_PORT->BSRR = (LED0_MASK << 16))
#  endif
#  define LED0_TOGGLE         (LED0_PORT->ODR  ^= LED0_MASK)
#endif

#if defined(LED1_PORT_NUM) && defined (LED1_PIN_NUM)
#  define LED1_PORT           ((GPIO_TypeDef *)GPIO_PORT(LED1_PORT_NUM))
#  define LED1_PIN            GPIO_PIN(LED1_PORT_NUM, LED1_PIN_NUM)
#  define LED1_MASK           (1 << LED1_PIN_NUM)
#  if IS_ACTIVE(LED1_IS_INVERTED)
#    define LED1_ON             (LED1_PORT->BSRR = (LED1_MASK << 16))
#    define LED1_OFF            (LED1_PORT->BSRR = LED1_MASK)
#  else
#    define LED1_ON             (LED1_PORT->BSRR = LED1_MASK)
#    define LED1_OFF            (LED1_PORT->BSRR = (LED1_MASK << 16))
#  endif
#  define LED1_TOGGLE         (LED1_PORT->ODR  ^= LED1_MASK)
#endif

#if defined(LED2_PORT_NUM) && defined (LED2_PIN_NUM)
#  define LED2_PORT           ((GPIO_TypeDef *)GPIO_PORT(LED2_PORT_NUM))
#  define LED2_PIN            GPIO_PIN(LED2_PORT_NUM, LED2_PIN_NUM)
#  define LED2_MASK           (1 << LED2_PIN_NUM)
#  if IS_ACTIVE(LED2_IS_INVERTED)
#    define LED2_ON             (LED2_PORT->BSRR = (LED2_MASK << 16))
#    define LED2_OFF            (LED2_PORT->BSRR = LED2_MASK)
#  else
#    define LED2_ON             (LED2_PORT->BSRR = LED2_MASK)
#    define LED2_OFF            (LED2_PORT->BSRR = (LED2_MASK << 16))
#  endif
#  define LED2_TOGGLE         (LED2_PORT->ODR  ^= LED2_MASK)
#endif

#if defined(LED3_PORT_NUM) && defined (LED3_PIN_NUM)
#  define LED3_PORT           ((GPIO_TypeDef *)GPIO_PORT(LED3_PORT_NUM))
#  define LED3_PIN            GPIO_PIN(LED3_PORT_NUM, LED3_PIN_NUM)
#  define LED3_MASK           (1 << LED3_PIN_NUM)
#  if IS_ACTIVE(LED3_IS_INVERTED)
#    define LED3_ON             (LED3_PORT->BSRR = (LED3_MASK << 16))
#    define LED3_OFF            (LED3_PORT->BSRR = LED3_MASK)
#  else
#    define LED3_ON             (LED3_PORT->BSRR = LED3_MASK)
#    define LED3_OFF            (LED3_PORT->BSRR = (LED3_MASK << 16))
#  endif
#  define LED3_TOGGLE         (LED3_PORT->ODR  ^= LED3_MASK)
#endif

#if defined(LED4_PORT_NUM) && defined (LED4_PIN_NUM)
#  define LED4_PORT           ((GPIO_TypeDef *)GPIO_PORT(LED4_PORT_NUM))
#  define LED4_PIN            GPIO_PIN(LED4_PORT_NUM, LED4_PIN_NUM)
#  define LED4_MASK           (1 << LED4_PIN_NUM)
#  if IS_ACTIVE(LED4_IS_INVERTED)
#    define LED4_ON             (LED4_PORT->BSRR = (LED4_MASK << 16))
#    define LED4_OFF            (LED4_PORT->BSRR = LED4_MASK)
#  else
#    define LED4_ON             (LED4_PORT->BSRR = LED4_MASK)
#    define LED4_OFF            (LED4_PORT->BSRR = (LED4_MASK << 16))
#  endif
#  define LED4_TOGGLE         (LED4_PORT->ODR  ^= LED4_MASK)
#endif

#if defined(LED5_PORT_NUM) && defined (LED5_PIN_NUM)
#  define LED5_PORT           ((GPIO_TypeDef *)GPIO_PORT(LED5_PORT_NUM))
#  define LED5_PIN            GPIO_PIN(LED5_PORT_NUM, LED5_PIN_NUM)
#  define LED5_MASK           (1 << LED5_PIN_NUM)
#  if IS_ACTIVE(LED5_IS_INVERTED)
#    define LED5_ON             (LED5_PORT->BSRR = (LED5_MASK << 16))
#    define LED5_OFF            (LED5_PORT->BSRR = LED5_MASK)
#  else
#    define LED5_ON             (LED5_PORT->BSRR = LED5_MASK)
#    define LED5_OFF            (LED5_PORT->BSRR = (LED5_MASK << 16))
#  endif
#  define LED5_TOGGLE         (LED5_PORT->ODR  ^= LED5_MASK)
#endif

#if defined(LED6_PORT_NUM) && defined (LED6_PIN_NUM)
#  define LED6_PORT           ((GPIO_TypeDef *)GPIO_PORT(LED6_PORT_NUM))
#  define LED6_PIN            GPIO_PIN(LED6_PORT_NUM, LED6_PIN_NUM)
#  define LED6_MASK           (1 << LED6_PIN_NUM)
#  if IS_ACTIVE(LED6_IS_INVERTED)
#    define LED6_ON             (LED6_PORT->BSRR = (LED6_MASK << 16))
#    define LED6_OFF            (LED6_PORT->BSRR = LED6_MASK)
#  else
#    define LED6_ON             (LED6_PORT->BSRR = LED6_MASK)
#    define LED6_OFF            (LED6_PORT->BSRR = (LED6_MASK << 16))
#  endif
#  define LED6_TOGGLE         (LED6_PORT->ODR  ^= LED6_MASK)
#endif

#if defined(LED7_PORT_NUM) && defined (LED7_PIN_NUM)
#  define LED7_PORT           ((GPIO_TypeDef *)GPIO_PORT(LED7_PORT_NUM))
#  define LED7_PIN            GPIO_PIN(LED7_PORT_NUM, LED7_PIN_NUM)
#  define LED7_MASK           (1 << LED7_PIN_NUM)
#  if IS_ACTIVE(LED7_IS_INVERTED)
#    define LED7_ON             (LED7_PORT->BSRR = (LED7_MASK << 16))
#    define LED7_OFF            (LED7_PORT->BSRR = LED7_MASK)
#  else
#    define LED7_ON             (LED7_PORT->BSRR = LED7_MASK)
#    define LED7_OFF            (LED7_PORT->BSRR = (LED7_MASK << 16))
#  endif
#  define LED7_TOGGLE         (LED7_PORT->ODR  ^= LED7_MASK)
#endif

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* STM32_LEDS_H */
/** @} */
