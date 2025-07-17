/*
 * SPDX-FileCopyrightText: 2016 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    boards_common_arduino_zero  Common files of Arduino Zero and clones
 * @ingroup     boards_common
 *
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Arduino Zero board and clones
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#include "cpu.h"
#include "periph_conf.h"
#include "periph_cpu.h"
#include "arduino_pinmap.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    xtimer configuration
 * @{
 */
#define XTIMER_DEV          TIMER_DEV(1)
#define XTIMER_CHAN         (0)
/** @} */

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#define LED0_PIN            GPIO_PIN(PA, 17)

#define LED_PORT            PORT->Group[PA]
#define LED0_MASK           (1 << 17)

#define LED0_ON             (LED_PORT.OUTSET.reg = LED0_MASK)
#define LED0_OFF            (LED_PORT.OUTCLR.reg = LED0_MASK)
#define LED0_TOGGLE         (LED_PORT.OUTTGL.reg = LED0_MASK)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
