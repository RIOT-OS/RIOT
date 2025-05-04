/*
 * Copyright (C) 2025 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_esp32_wemos_d1_r32
 * @brief       Board specific definitions for Wemos D1 R32 (ESPDuino-32) board
 * @{
 *
 * For detailed information about the configuration of ESP32 boards, see
 * section \ref esp32_peripherals "Common Peripherals".
 *
 * @file
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>

/**
 * @name    Button pin definitions
 * @{
 */

/**
 * @brief   Default button GPIO pin definition
 *
 * The Wemos D1 R32 board which is exactly the same as the ESPDuino-32 board
 * has a BOOT button connected to GPIO0, which can be used as button during
 * normal operation.
 */
#define BTN0_PIN        GPIO0

/**
 * @brief   Default button GPIO mode definition
 */
#define BTN0_MODE       GPIO_IN_PU

/**
 * @brief   Default interrupt flank definition for the button GPIO
 *
 * Since the GPIO of the button is pulled up with an external resistor, the
 * mode for the GPIO pin has to be GPIO_IN.
 */
#ifndef BTN0_INT_FLANK
#define BTN0_INT_FLANK  GPIO_FALLING
#endif

/**
 * @brief   Definition for compatibility with previous versions
 */
#define BUTTON0_PIN     BTN0_PIN

/** @} */

/**
 * @name    LED (on-board) configuration
 * @{
 */
#define LED0_PIN        GPIO2   /**< LED is connected to GPIO2 */
#define LED0_ACTIVE     1       /**< LED is high active */
/** @} */

/* include common board definitions as last step */
#include "board_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* BOARD_H */
/** @} */
