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
 * The idea is that STM32 boards only define the pin number, port number and
 * port of LEDs and this header provides the rest of the defines
 */

#ifndef STM32_LEDS_H
#define STM32_LEDS_H

/* Using gpio_ll_arch for GPIO access. On STM32, accessing the GPIO output
 * buffer via GPIO LL even works when the GPIO LL feature/module is not in
 * use */
#include "periph/gpio_ll.h"
#include "kernel_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Common LED pin definitions for STM32 boards
 * @{
 */
#if defined(LED0_PORT_NUM) && defined (LED0_PIN_NUM)
#  define LED0_PIN              GPIO_PIN(LED0_PORT_NUM, LED0_PIN_NUM)
#  define LED0_MASK             (1 << LED0_PIN_NUM)
#  if IS_ACTIVE(LED0_IS_INVERTED)
#    define LED0_ON             gpio_ll_clear(LED0_PORT, LED0_MASK)
#    define LED0_OFF            gpio_ll_set(LED0_PORT, LED0_MASK)
#  else
#    define LED0_ON             gpio_ll_set(LED0_PORT, LED0_MASK)
#    define LED0_OFF            gpio_ll_clear(LED0_PORT, LED0_MASK)
#  endif
#  define LED0_TOGGLE           gpio_ll_toggle(LED0_PORT, LED0_MASK)
#endif

#if defined(LED1_PORT_NUM) && defined (LED1_PIN_NUM)
#  define LED1_PIN              GPIO_PIN(LED1_PORT_NUM, LED1_PIN_NUM)
#  define LED1_MASK             (1 << LED1_PIN_NUM)
#  if IS_ACTIVE(LED1_IS_INVERTED)
#    define LED1_ON             gpio_ll_clear(LED1_PORT, LED1_MASK)
#    define LED1_OFF            gpio_ll_set(LED1_PORT, LED1_MASK)
#  else
#    define LED1_ON             gpio_ll_set(LED1_PORT, LED1_MASK)
#    define LED1_OFF            gpio_ll_clear(LED1_PORT, LED1_MASK)
#  endif
#  define LED1_TOGGLE           gpio_ll_toggle(LED1_PORT, LED1_MASK)
#endif

#if defined(LED2_PORT_NUM) && defined (LED2_PIN_NUM)
#  define LED2_PIN              GPIO_PIN(LED2_PORT_NUM, LED2_PIN_NUM)
#  define LED2_MASK             (1 << LED2_PIN_NUM)
#  if IS_ACTIVE(LED2_IS_INVERTED)
#    define LED2_ON             gpio_ll_clear(LED2_PORT, LED2_MASK)
#    define LED2_OFF            gpio_ll_set(LED2_PORT, LED2_MASK)
#  else
#    define LED2_ON             gpio_ll_set(LED2_PORT, LED2_MASK)
#    define LED2_OFF            gpio_ll_clear(LED2_PORT, LED2_MASK)
#  endif
#  define LED2_TOGGLE           gpio_ll_toggle(LED2_PORT, LED2_MASK)
#endif

#if defined(LED3_PORT_NUM) && defined (LED3_PIN_NUM)
#  define LED3_PIN              GPIO_PIN(LED3_PORT_NUM, LED3_PIN_NUM)
#  define LED3_MASK             (1 << LED3_PIN_NUM)
#  if IS_ACTIVE(LED3_IS_INVERTED)
#    define LED3_ON             gpio_ll_clear(LED3_PORT, LED3_MASK)
#    define LED3_OFF            gpio_ll_set(LED3_PORT, LED3_MASK)
#  else
#    define LED3_ON             gpio_ll_set(LED3_PORT, LED3_MASK)
#    define LED3_OFF            gpio_ll_clear(LED3_PORT, LED3_MASK)
#  endif
#  define LED3_TOGGLE           gpio_ll_toggle(LED3_PORT, LED3_MASK)
#endif

#if defined(LED4_PORT_NUM) && defined (LED4_PIN_NUM)
#  define LED4_PIN              GPIO_PIN(LED4_PORT_NUM, LED4_PIN_NUM)
#  define LED4_MASK             (1 << LED4_PIN_NUM)
#  if IS_ACTIVE(LED4_IS_INVERTED)
#    define LED4_ON             gpio_ll_clear(LED4_PORT, LED4_MASK)
#    define LED4_OFF            gpio_ll_set(LED4_PORT, LED4_MASK)
#  else
#    define LED4_ON             gpio_ll_set(LED4_PORT, LED4_MASK)
#    define LED4_OFF            gpio_ll_clear(LED4_PORT, LED4_MASK)
#  endif
#  define LED4_TOGGLE           gpio_ll_toggle(LED4_PORT, LED4_MASK)
#endif

#if defined(LED5_PORT_NUM) && defined (LED5_PIN_NUM)
#  define LED5_PIN              GPIO_PIN(LED5_PORT_NUM, LED5_PIN_NUM)
#  define LED5_MASK             (1 << LED5_PIN_NUM)
#  if IS_ACTIVE(LED5_IS_INVERTED)
#    define LED5_ON             gpio_ll_clear(LED5_PORT, LED5_MASK)
#    define LED5_OFF            gpio_ll_set(LED5_PORT, LED5_MASK)
#  else
#    define LED5_ON             gpio_ll_set(LED5_PORT, LED5_MASK)
#    define LED5_OFF            gpio_ll_clear(LED5_PORT, LED5_MASK)
#  endif
#  define LED5_TOGGLE           gpio_ll_toggle(LED5_PORT, LED5_MASK)
#endif

#if defined(LED6_PORT_NUM) && defined (LED6_PIN_NUM)
#  define LED6_PIN              GPIO_PIN(LED6_PORT_NUM, LED6_PIN_NUM)
#  define LED6_MASK             (1 << LED6_PIN_NUM)
#  if IS_ACTIVE(LED6_IS_INVERTED)
#    define LED6_ON             gpio_ll_clear(LED6_PORT, LED6_MASK)
#    define LED6_OFF            gpio_ll_set(LED6_PORT, LED6_MASK)
#  else
#    define LED6_ON             gpio_ll_set(LED6_PORT, LED6_MASK)
#    define LED6_OFF            gpio_ll_clear(LED6_PORT, LED6_MASK)
#  endif
#  define LED6_TOGGLE           gpio_ll_toggle(LED6_PORT, LED6_MASK)
#endif

#if defined(LED7_PORT_NUM) && defined (LED7_PIN_NUM)
#  define LED7_PIN              GPIO_PIN(LED7_PORT_NUM, LED7_PIN_NUM)
#  define LED7_MASK             (1 << LED7_PIN_NUM)
#  if IS_ACTIVE(LED7_IS_INVERTED)
#    define LED7_ON             gpio_ll_clear(LED7_PORT, LED7_MASK)
#    define LED7_OFF            gpio_ll_set(LED7_PORT, LED7_MASK)
#  else
#    define LED7_ON             gpio_ll_set(LED7_PORT, LED7_MASK)
#    define LED7_OFF            gpio_ll_clear(LED7_PORT, LED7_MASK)
#  endif
#  define LED7_TOGGLE           gpio_ll_toggle(LED7_PORT, LED7_MASK)
#endif

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* STM32_LEDS_H */
/** @} */
