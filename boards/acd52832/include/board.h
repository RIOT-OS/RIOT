/*
 * SPDX-FileCopyrightText: 2017 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_acd52832
 *
 * For more information: http://aconno.de/acd52832/
 *
 * @{
 *
 * @file
 * @brief       Board specific configuration for the ACD52832
 *
 * @author      Dimitri Nahm <dimitri.nahm@haw-hamburg.de>
 */

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    LED pin configuration
 * @{
 */
#define LED0_PIN            GPIO_PIN(0, 26)

#define LED_PORT            (NRF_P0)
#define LED0_MASK           (1 << 26)

#define LED0_ON             (LED_PORT->OUTCLR = LED0_MASK)
#define LED0_OFF            (LED_PORT->OUTSET = LED0_MASK)
#define LED0_TOGGLE         (LED_PORT->OUT   ^= LED0_MASK)
/** @} */

/**
 * @name    Button pin configuration
 * @{
 */
#define BTN0_PIN            GPIO_PIN(0, 25)
#define BTN0_MODE           GPIO_IN_PU
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
