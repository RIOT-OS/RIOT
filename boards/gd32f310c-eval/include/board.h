/*
 * Copyright (C) 2023 BISSELL Homecare, Inc.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_gd32f310c-eval
 * @{
 *
 * @file
 * @brief       Board specific definitions for the GD32F310C-EVAL
 *
 * @author      Jason Parker <jason.parker@bissell.com>
 */

#ifndef BOARD_H
#define BOARD_H

//#include "cpu.h"
//#include "periph_conf.h"
//#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#define LED0_BIT            8
#define LED0_GPIO_PORT      PORT_A
// #define LED0_GPIO_CLK       RCU_GPIOA
#define LED0_PIN            GPIO_PIN(LED0_GPIO_PORT, LED0_BIT)
#define LED0_MASK           (1 << LED0_BIT)
#define LED0_ON             (GPIOA->BOP = LED0_MASK)
#define LED0_OFF            (GPIOA->BC = LED0_MASK)
#define LED0_TOGGLE         (GPIOA->TG = LED0_MASK)

#define LED1_BIT            11
#define LED1_GPIO_PORT      PORT_A
// #define LED1_GPIO_CLK       RCU_GPIOA
#define LED1_PIN            GPIO_PIN(LED1_GPIO_PORT, LED1_BIT)
#define LED1_MASK           (1 << LED1_BIT)
#define LED1_ON             (GPIOA->BOP = LED1_MASK)
#define LED1_OFF            (GPIOA->BC = LED1_MASK)
#define LED1_TOGGLE         (GPIOA->TG = LED1_MASK)

#define LED2_BIT            12
#define LED2_GPIO_PORT      PORT_A
// #define LED2_GPIO_CLK       RCU_GPIOA
#define LED2_PIN            GPIO_PIN(LED2_GPIO_PORT, LED2_BIT)
#define LED2_MASK           (1 << LED2_BIT)
#define LED2_ON             (GPIOA->BOP = LED2_MASK)
#define LED2_OFF            (GPIOA->BC = LED2_MASK)
#define LED2_TOGGLE         (GPIOA->TG = LED2_MASK)

#define LED3_BIT            15
#define LED3_GPIO_PORT      PORT_A
// #define LED3_GPIO_CLK       RCU_GPIOA
#define LED3_PIN            GPIO_PIN(LED3_GPIO_PORT, LED3_BIT)
#define LED3_MASK           (1 << LED3_BIT)
#define LED3_ON             (GPIOA->BOP = LED3_MASK)
#define LED3_OFF            (GPIOA->BC = LED3_MASK)
#define LED3_TOGGLE         (GPIOA->TG = LED3_MASK)

// #define LEDx_GPIO_PORT      GPIOA    //  <<< should be used in _ON _OFF _TOGGLE instead of restated
// #define LEDx_ON             (GPIO_BOP(LEDx_GPIO_PORT) = (uint32_t) LEDx_MASK)
// #define LEDx_OFF            (GPIO_BC(LEDx_GPIO_PORT) = (uint32_t) LEDx_MASK)
// #define LEDx_TOGGLE         (GPIO_TG(LEDx_GPIO_PORT) = (uint32_t) LEDx_MASK)

/* wakeup push-button */
#define BTN0_BIT            0
#define BTN0_GPIO_PORT      PORT_A
// #define BTN0_GPIO_CLK       RCU_GPIOA
#define BTN0_PIN            GPIO_PIN(BTN0_GPIO_PORT, BTN0_BIT)
#define BTN0_MASK           (1 << BTN0_BIT)
#define BTN0_MODE           GPIO_IN

/* "tamper" push-button */
#define BTN1_BIT            13
#define BTN1_GPIO_PORT      PORT_C
// #define BTN1_GPIO_CLK       RCU_GPIOC
#define BTN1_PIN            GPIO_PIN(BTN1_GPIO_PORT, BTN1_BIT)
#define BTN1_MASK           (1 << BTN1_BIT)
#define BTN1_MODE           GPIO_IN

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
