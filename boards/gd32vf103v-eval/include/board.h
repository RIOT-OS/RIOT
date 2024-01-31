/*
 * Copyright (C) 2023 BISSELL Homecare, Inc.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_gd32vf103v-eval
 * @{
 *
 * @file
 * @brief       Board specific definitions for the GD32VF103V-EVAL
 *
 * @author      Jason Parker <jason.parker@bissell.com>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "periph_conf.h"
#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#define LED0_BIT            0
#define LED0_GPIO_PORT      PORT_C
#define LED0_PIN            GPIO_PIN(LED0_GPIO_PORT, LED0_BIT)
#define LED0_MASK           (1 << LED0_BIT)
#define LED0_ON             (GPIOC->BOP = LED0_MASK)
#define LED0_OFF            (GPIOC->BC = LED0_MASK)
#define LED0_TOGGLE         (GPIOC->OCTL ^= LED0_MASK)

#define LED1_BIT            2
#define LED1_GPIO_PORT      PORT_C
#define LED1_PIN            GPIO_PIN(LED1_GPIO_PORT, LED1_BIT)
#define LED1_MASK           (1 << LED1_BIT)
#define LED1_ON             (GPIOC->BOP = LED1_MASK)
#define LED1_OFF            (GPIOC->BC = LED1_MASK)
#define LED1_TOGGLE         (GPIOC->OCTL ^= LED1_MASK)

#define LED2_BIT            0
#define LED2_GPIO_PORT      PORT_E
#define LED2_PIN            GPIO_PIN(LED2_GPIO_PORT, LED2_BIT)
#define LED2_MASK           (1 << LED2_BIT)
#define LED2_ON             (GPIOE->BOP = LED2_MASK)
#define LED2_OFF            (GPIOE->BC = LED2_MASK)
#define LED2_TOGGLE         (GPIOE->OCTL ^= LED2_MASK)

#define LED3_BIT            1
#define LED3_GPIO_PORT      PORT_E
#define LED3_PIN            GPIO_PIN(LED3_GPIO_PORT, LED3_BIT)
#define LED3_MASK           (1 << LED3_BIT)
#define LED3_ON             (GPIOE->BOP = LED3_MASK)
#define LED3_OFF            (GPIOE->BC = LED3_MASK)
#define LED3_TOGGLE         (GPIOE->OCTL ^= LED3_MASK)

/* A */
#define BTN0_BIT            0
#define BTN0_GPIO_PORT      PORT_A
#define BTN0_PIN            GPIO_PIN(BTN0_GPIO_PORT, BTN0_BIT)
#define BTN0_MASK           (1 << BTN0_BIT)
#define BTN0_MODE           GPIO_IN
#define BTN0_INT_FLANK      GPIO_RISING

/* B */
#define BTN1_BIT            13
#define BTN1_GPIO_PORT      PORT_C
#define BTN1_PIN            GPIO_PIN(BTN1_GPIO_PORT, BTN1_BIT)
#define BTN1_MASK           (1 << BTN1_BIT)
#define BTN1_MODE           GPIO_IN
#define BTN1_INT_FLANK      GPIO_RISING

/* C */
#define BTN2_BIT            14
#define BTN2_GPIO_PORT      PORT_B
#define BTN2_PIN            GPIO_PIN(BTN2_GPIO_PORT, BTN2_BIT)
#define BTN2_MASK           (1 << BTN2_BIT)
#define BTN2_MODE           GPIO_IN
#define BTN2_INT_FLANK      GPIO_RISING

/* D */
#define BTN3_BIT            5
#define BTN3_GPIO_PORT      PORT_C
#define BTN3_PIN            GPIO_PIN(BTN3_GPIO_PORT, BTN3_BIT)
#define BTN3_MASK           (1 << BTN3_BIT)
#define BTN3_MODE           GPIO_IN
#define BTN3_INT_FLANK      GPIO_RISING

/* Cet */
#define BTN4_BIT            4
#define BTN4_GPIO_PORT      PORT_C
#define BTN4_PIN            GPIO_PIN(BTN4_GPIO_PORT, BTN4_BIT)
#define BTN4_MASK           (1 << BTN4_BIT)
#define BTN4_MODE           GPIO_IN
#define BTN4_INT_FLANK      GPIO_RISING

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
