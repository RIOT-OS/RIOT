/*
 * Copyright (C) 2017 Hamburg University of Applied Sciences
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     pkg_openwsn
 * @{
 *
 * @file
 * @brief       Default configuration for the OpenWSN leds
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 *
 * @}
 */

#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    OpenWSN default leds pins configuration.
 *
 * Default settings match OpenWSN openmote-b configuration.
 *
 * @note On Nucleo boards the LED pin is shared with SPI -> don't use it!
 * @{
 */
#ifndef OPENWSN_LEDPIN_ERROR
#if defined (LED0_PIN) && !defined(MODULE_BOARDS_COMMON_NUCLEO)
#define OPENWSN_LEDPIN_ERROR            LED0_PIN
#else
#define OPENWSN_LEDPIN_ERROR            GPIO_UNDEF
#endif
#endif

#ifndef OPENWSN_LEDPIN_SYNC
#ifdef LED1_PIN
#define OPENWSN_LEDPIN_SYNC             LED1_PIN
#else
#define OPENWSN_LEDPIN_SYNC             GPIO_UNDEF
#endif
#endif

#ifndef OPENWSN_LEDPIN_RADIO
#ifdef LED2_PIN
#define OPENWSN_LEDPIN_RADIO            LED2_PIN
#else
#define OPENWSN_LEDPIN_RADIO            GPIO_UNDEF
#endif
#endif

#ifndef OPENWSN_LEDPIN_DEBUG
#ifdef LED3_PIN
#define OPENWSN_LEDPIN_DEBUG            LED3_PIN
#else
#define OPENWSN_LEDPIN_DEBUG            GPIO_UNDEF
#endif
#endif

#ifndef OPENWSN_LED_ON_STATE
#define OPENWSN_LED_ON_STATE             GPIO_LED_LOW
#endif

#define OPENWSN_LEDS_DEFAULT    { .error = OPENWSN_LEDPIN_ERROR, \
                                  .sync = OPENWSN_LEDPIN_SYNC, \
                                  .radio = OPENWSN_LEDPIN_RADIO, \
                                  .debug = OPENWSN_LEDPIN_DEBUG, \
                                  .led_on = OPENWSN_LED_ON_STATE }
/**@}*/

/**
 * @brief   OpenWSN leds configuration
 */
static const leds_config_t openwsn_leds_params[] =
{
#ifdef OPENWSN_LEDS_BOARD
    OPENWSN_LEDS_BOARD,
#else
    OPENWSN_LEDS_DEFAULT,
#endif
};

#ifdef __cplusplus
}
#endif
