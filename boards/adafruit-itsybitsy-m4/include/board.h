/*
 * Copyright (C) 2021 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     boards_adafruit-itsybitsy-m4
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Adafruit ItsyBitsy M4
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#include "cpu.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   LED pin definitions and handlers
 * @{
 */
#define LED0_PIN            GPIO_PIN(PA, 22)

#define LED_PORT            PORT->Group[PA]
#define LED0_MASK           (1 << 22)

#define LED0_ON             (LED_PORT.OUTSET.reg = LED0_MASK)
#define LED0_OFF            (LED_PORT.OUTCLR.reg = LED0_MASK)
#define LED0_TOGGLE         (LED_PORT.OUTTGL.reg = LED0_MASK)
/** @} */

/**
 * @name APA102 / DotStar configuration
 * @{
 */
#define APA102_PARAM_LED_NUMOF      (1)
#define APA102_PARAM_DATA_PIN       GPIO_PIN(PB, 3)
#define APA102_PARAM_CLK_PIN        GPIO_PIN(PB, 2)
/** @} */

/**
 * @name MTD configuration
 * @{
 */
#define MTD_0 mtd_dev_get(0)
/** @} */

/**
 * @name    Xtimer configuration
 * @{
 */
#define XTIMER_WIDTH                (32)
#define XTIMER_HZ                   (1000000ul)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
