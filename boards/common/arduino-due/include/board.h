/*
 * SPDX-FileCopyrightText: 2014 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2017 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    boards_common_arduino_due Arduino Due Common
 * @ingroup     boards_common
 * @brief       Shared files and configuration for Arduino Due based boards.
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Arduino Due based boards
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#include "cpu.h"
#include "arduino_pinmap.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name LED pin definitions
 * @{
 */
#define LED0_PIN            GPIO_PIN(PB, 27)

#define LED0_ON             (PIOB->PIO_SODR  = PIO_PB27)
#define LED0_OFF            (PIOB->PIO_CODR  = PIO_PB27)
#define LED0_TOGGLE         ((PIOB->PIO_ODSR & PIO_PB27) ? LED0_OFF : LED0_ON)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
