/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     boards_nrf52832-mdk
 * @{
 *
 * @file
 * @brief       Board specific configuration for the nRF52832-MDK
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#include "cpu.h"
#include "board_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    LED pin configuration
 * @{
 */
#define LED0_PIN            GPIO_PIN(0, 23)
#define LED1_PIN            GPIO_PIN(0, 22)
#define LED2_PIN            GPIO_PIN(0, 24)

#define LED_PORT            (NRF_P0)
#define LED0_MASK           (1 << 23)
#define LED1_MASK           (1 << 22)
#define LED2_MASK           (1 << 24)
#define LED_MASK            (LED0_MASK | LED1_MASK | LED2_MASK)

#define LED0_ON             (LED_PORT->OUTCLR = LED0_MASK)
#define LED0_OFF            (LED_PORT->OUTSET = LED0_MASK)
#define LED0_TOGGLE         (LED_PORT->OUT   ^= LED0_MASK)

#define LED1_ON             (LED_PORT->OUTCLR = LED1_MASK)
#define LED1_OFF            (LED_PORT->OUTSET = LED1_MASK)
#define LED1_TOGGLE         (LED_PORT->OUT   ^= LED1_MASK)

#define LED2_ON             (LED_PORT->OUTCLR = LED2_MASK)
#define LED2_OFF            (LED_PORT->OUTSET = LED2_MASK)
#define LED2_TOGGLE         (LED_PORT->OUT   ^= LED2_MASK)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
