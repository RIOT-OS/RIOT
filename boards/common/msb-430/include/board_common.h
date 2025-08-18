/*
 * SPDX-FileCopyrightText: 2013-2016 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    boards_common_msb-430 MSB-430 common
 * @ingroup     boards_common
 * @brief       Shared files and configuration for all MSB-430 boards.
 * @{
 *
 * @file
 * @brief       Common definitions for all msb-430 based boards
 *
 * @author      unknown
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Xtimer configuration
 * @{
 */
#define XTIMER_WIDTH                (16)
#define XTIMER_BACKOFF              (40)
/** @} */

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#define LED0_PIN                    GPIO_PIN(4, 7)

#define LED_OUT_REG                 (P5OUT)
#define LED0_MASK                   (1 << 7)

#define LED0_ON                     (LED_OUT_REG &= ~LED0_MASK)
#define LED0_OFF                    (LED_OUT_REG |=  LED0_MASK)
#define LED0_TOGGLE                 (LED_OUT_REG ^=  LED0_MASK)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
