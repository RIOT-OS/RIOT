/*
 * Copyright (C) 2023 BISSELL Homecare, Inc.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_gd32e230c-eval
 * @{
 *
 * @file
 * @brief       Board specific definitions for the GD32E230C-EVAL
 *
 * @author      Jason Parker <jason.parker@bissell.com>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "periph_conf.h"
#include "periph_cpu.h"
#include "gd32e23x_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#define LED0_BIT            8
#define LED0_GPIO_PORT      GPIOA
#define LED0_GPIO_CLK       RCU_GPIOA
#define LED0_PIN            GPIO_PIN(LED0_GPIO_PORT, LED0_BIT)
#define LED0_MASK           (1 << LED0_BIT)
#define LED0_ON             (GPIO_BOP(LED0_GPIO_PORT) = (uint32_t) LED0_MASK)
#define LED0_OFF            (GPIO_BC(LED0_GPIO_PORT) = (uint32_t) LED0_MASK)
#define LED0_TOGGLE         (GPIO_TG(LED0_GPIO_PORT) = (uint32_t) LED0_MASK)

#define LED1_BIT            11
#define LED1_GPIO_PORT      GPIOA
#define LED1_GPIO_CLK       RCU_GPIOA
#define LED1_PIN            GPIO_PIN(LED1_GPIO_PORT, LED1_BIT)
#define LED1_MASK           (1 << LED1_BIT)
#define LED1_ON             (GPIO_BOP(LED1_GPIO_PORT) = (uint32_t) LED1_MASK)
#define LED1_OFF            (GPIO_BC(LED1_GPIO_PORT) = (uint32_t) LED1_MASK)
#define LED1_TOGGLE         (GPIO_TG(LED1_GPIO_PORT) = (uint32_t) LED1_MASK)

#define LED2_BIT            12
#define LED2_GPIO_PORT      GPIOA
#define LED2_GPIO_CLK       RCU_GPIOA
#define LED2_PIN            GPIO_PIN(LED2_GPIO_PORT, LED2_BIT)
#define LED2_MASK           (1 << LED2_BIT)
#define LED2_ON             (GPIO_BOP(LED2_GPIO_PORT) = (uint32_t) LED2_MASK)
#define LED2_OFF            (GPIO_BC(LED2_GPIO_PORT) = (uint32_t) LED2_MASK)
#define LED2_TOGGLE         (GPIO_TG(LED2_GPIO_PORT) = (uint32_t) LED2_MASK)

#define LED3_BIT            15
#define LED3_GPIO_PORT      GPIOA
#define LED3_GPIO_CLK       RCU_GPIOA
#define LED3_PIN            GPIO_PIN(LED3_GPIO_PORT, LED3_BIT)
#define LED3_MASK           (1 << LED3_BIT)
#define LED3_ON             (GPIO_BOP(LED3_GPIO_PORT) = (uint32_t) LED3_MASK)
#define LED3_OFF            (GPIO_BC(LED3_GPIO_PORT) = (uint32_t) LED3_MASK)
#define LED3_TOGGLE         (GPIO_TG(LED3_GPIO_PORT) = (uint32_t) LED3_MASK)

/* "wakeup" push-button */
#define KEY0_BIT            0
#define KEY0_GPIO_PORT      GPIOA
#define KEY0_GPIO_CLK       RCU_GPIOA
#define KEY0_PIN            GPIO_PIN(KEY0_GPIO_PORT, KEY0_BIT)
#define KEY0_MASK           (1 << KEY0_BIT)
#define BTN0_PIN            KEY0_PIN
#define BTN0_MODE           GPIO_IN

/* "tamper" push-button */
#define KEY1_BIT            13
#define KEY1_GPIO_PORT      GPIOC
#define KEY1_GPIO_CLK       RCU_GPIOC
#define KEY1_PIN            GPIO_PIN(KEY1_GPIO_PORT, KEY1_BIT)
#define KEY1_MASK           (1 << KEY1_BIT)
#define BTN1_PIN            KEY1_PIN
#define BTN1_MODE           GPIO_IN

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
