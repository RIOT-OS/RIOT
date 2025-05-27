/*
 * Copyright (C) 2025 Mesotic SAS
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     boards_sam4s-xpro
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Microchip SAM 4S Xplained Pro
 *              board
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 */

#include "cpu.h"
#include "periph_conf.h"
#include "periph_cpu.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    ztimer configuration
 * @{
 */
#define CONFIG_ZTIMER_USEC_WIDTH        (16)
/** @} */

/**
 * @name   LED pin definitions and handlers
 * @{
 */
#define LED0_PIN            GPIO_PIN(PC, 23)

#define LED0_ON             (PIOC->PIO_CODR  = PIO_PC23)
#define LED0_OFF            (PIOC->PIO_SODR  = PIO_PC23)
#define LED0_TOGGLE         ((PIOC->PIO_ODSR & PIO_PC23) ? LED0_ON : LED0_OFF)
/** @} */

/**
 * @name SW0 (Button) pin definitions
 * @{
 */
#define BTN0_PIN            GPIO_PIN(PA, 2)
#define BTN0_MODE           GPIO_IN_PU
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
