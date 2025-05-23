/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     boards_nrf51dongle
 * @{
 *
 * @file
 * @brief       Board specific configuration for the nRF51 Dongle
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#include "board_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#define LED0_PIN            GPIO_PIN(0, 21)
#define LED1_PIN            GPIO_PIN(0, 22)
#define LED2_PIN            GPIO_PIN(0, 23)

#define LED0_MASK           (1 << 21)
#define LED1_MASK           (1 << 22)
#define LED2_MASK           (1 << 23)

#define LED0_ON             (NRF_GPIO->OUTCLR = LED0_MASK)
#define LED0_OFF            (NRF_GPIO->OUTSET = LED0_MASK)
#define LED0_TOGGLE         (NRF_GPIO->OUT   ^= LED0_MASK)

#define LED1_ON             (NRF_GPIO->OUTCLR = LED1_MASK)
#define LED1_OFF            (NRF_GPIO->OUTSET = LED1_MASK)
#define LED1_TOGGLE         (NRF_GPIO->OUT   ^= LED1_MASK)

#define LED2_ON             (NRF_GPIO->OUTCLR = LED2_MASK)
#define LED2_OFF            (NRF_GPIO->OUTSET = LED2_MASK)
#define LED2_TOGGLE         (NRF_GPIO->OUT   ^= LED2_MASK)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
