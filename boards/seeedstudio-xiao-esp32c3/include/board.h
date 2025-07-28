/*
 * SPDX-FileCopyrightText: 2025 David Picard
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_seeedstudio-xiao-esp32c3
 * @brief       Board definitions for the Seeed Studio Xiao ESP32-C3 board
 * @{
 *
 * @file        board.h
 * @author      David Picard
 */

#include <stdint.h>

#if MODULE_PERIPH_INIT_BUTTONS || DOXYGEN
/**
 * @name    Button pin definitions
 * @{
 */

/**
 * @brief   Default button GPIO pin definition
 *
 * Pressing the button will give a low signal.
 *
 * @note GPIO9 is a strapping pin that must be pulled up a boot time
 *       in order to boot the user application.
 *       After boot, it can be used as user button.
 *
 * @note \c BTN0_PIN conflicts with the SPI MISO line. If the SPI
 *       module is enabled, the button will be automatically disabled.
 */
#define BTN0_PIN        GPIO9

/**
 * @brief   Default button GPIO mode definition
 *
 * The pîn is actually pulled up by an external resistor on the board.
 * As a consequence, the pin mode should be #GPIO_IN.
 * The internal pull-up resistor is not enabled on purpose
 * because it would decrease the total pull-up resistor value.
 */
#define BTN0_MODE       GPIO_IN

/**
 * @brief   Default interrupt flank definition for the button GPIO
 */
#ifndef BTN0_INT_FLANK
#define BTN0_INT_FLANK  GPIO_FALLING
#endif

/**
 * @brief   Definition for compatibility with previous versions
 */
#define BUTTON0_PIN     BTN0_PIN

/** @} */
#endif

/* include common board definitions as last step */
#include "board_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
} /* end extern "C" */
#endif

/** @} */
