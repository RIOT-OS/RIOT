/*
 * SPDX-FileCopyrightText: 2025 David Picard
 * SPDX-FileCopyrightText: 2026 Stepan Konoplev
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_seeedstudio-xiao-esp32c6
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Seeed Studio XIAO ESP32-C6
 *
 * @author      Stepan Konoplev <stepan.konoplev@haw-hamburg.de>
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    LED configuration
 * @{
 */
#define LED0_PIN        GPIO15
#define LED0_ACTIVE     (0)     /**< LED is low active */
/** @} */

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
 * @note GPIO9 is a strapping pin that must be pulled up at boot time
 *       in order to boot the user application.
 *       After boot, it can be used as user button.
 */
#  define BTN0_PIN        GPIO9
/**
 * @brief   Default button GPIO mode definition
 *
 * The pin is actually pulled up by an external resistor on the board.
 * As a consequence, the pin mode should be GPIO_IN.
 * The internal pull-up resistor is not enabled on purpose
 * because it would decrease the total pull-up resistor value.
 */
#  define BTN0_MODE       GPIO_IN
/**
 * @brief   Default interrupt flank definition for the button GPIO
 */
#  ifndef BTN0_INT_FLANK
#    define BTN0_INT_FLANK  GPIO_FALLING
#  endif
/**
 * @brief   Definition for compatibility with previous versions
 */
#  define BUTTON0_PIN     BTN0_PIN
/** @} */
#endif

/**
 * @name    RF antenna switch configuration
 * @{
 */
#define RF_SWITCH_EN_GPIO   GPIO3    /**< RF switch enable, active low */
#define RF_SWITCH_ANT_GPIO  GPIO14   /**< antenna select: low=internal, high=external */
/** @} */

#include "board_common.h"

#ifdef __cplusplus
}
#endif

/** @} */
