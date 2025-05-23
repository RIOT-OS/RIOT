/*
 * Copyright (C) 2014 Loci Controls Inc.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     boards_cc2538dk
 * @{
 *
 * @file
 *
 * @author      Ian Martin <ian@locicontrols.com>
 */

#include "cpu.h"
#include "periph/gpio.h"
#include "cc2538_eui_primary.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#define LED0_PIN            GPIO_PIN(2, 0)
#define LED1_PIN            GPIO_PIN(2, 1)
#define LED2_PIN            GPIO_PIN(2, 2)
#define LED3_PIN            GPIO_PIN(2, 3)

#define LED_PORT            GPIO_C
#define LED0_BIT            (1 << 0)        /**< red LED */
#define LED1_BIT            (1 << 1)        /**< yellow LED */
#define LED2_BIT            (1 << 2)        /**< green LED */
#define LED3_BIT            (1 << 3)        /**< orange LED */

#define LED0_ON             (LED_PORT->DATA |=  LED0_BIT)
#define LED0_OFF            (LED_PORT->DATA &= ~LED0_BIT)
#define LED0_TOGGLE         (LED_PORT->DATA ^=  LED0_BIT)

#define LED1_ON             (LED_PORT->DATA |=  LED1_BIT)
#define LED1_OFF            (LED_PORT->DATA &= ~LED1_BIT)
#define LED1_TOGGLE         (LED_PORT->DATA ^=  LED1_BIT)

#define LED2_ON             (LED_PORT->DATA |=  LED2_BIT)
#define LED2_OFF            (LED_PORT->DATA &= ~LED2_BIT)
#define LED2_TOGGLE         (LED_PORT->DATA ^=  LED2_BIT)

#define LED3_ON             (LED_PORT->DATA |=  LED3_BIT)
#define LED3_OFF            (LED_PORT->DATA &= ~LED3_BIT)
#define LED3_TOGGLE         (LED_PORT->DATA ^=  LED3_BIT)
/** @} */

/**
 * @name    Flash Customer Configuration Area (CCA) parameters
 * @{
 */
#ifndef UPDATE_CCA
#define UPDATE_CCA                1
#endif

#define CCA_BACKDOOR_ENABLE       1
#define CCA_BACKDOOR_PORT_A_PIN   3 /**< Select button */
#define CCA_BACKDOOR_ACTIVE_LEVEL 0 /**< Active low */
/** @} */

/**
 * @name    xtimer configuration
 * @{
 */
#define XTIMER_WIDTH        (16)
#define XTIMER_BACKOFF      (50)
#define XTIMER_ISR_BACKOFF  (40)
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

/** @} */
