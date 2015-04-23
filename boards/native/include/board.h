/*
 * Copyright (C) 2013 Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_native Native Board
 * @ingroup     boards
 * @brief       Board emulation for the native port
 *
 * The native board uses call level hardware simulation
 *
 * @{
 *
 * @file
 * @brief       Basic definitions for the native board
 *
 * @author      Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 */

#ifndef BOARD_H_
#define BOARD_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define F_CPU 1000000

void _native_LED_GREEN_OFF(void);
#define LED_GREEN_OFF (_native_LED_GREEN_OFF())
void _native_LED_GREEN_ON(void);
#define LED_GREEN_ON (_native_LED_GREEN_ON())
void _native_LED_GREEN_TOGGLE(void);
#define LED_GREEN_TOGGLE (_native_LED_GREEN_TOGGLE())
void _native_LED_RED_OFF(void);
#define LED_RED_OFF (_native_LED_RED_OFF())
void _native_LED_RED_ON(void);
#define LED_RED_ON (_native_LED_RED_ON())
void _native_LED_RED_TOGGLE(void);
#define LED_RED_TOGGLE (_native_LED_RED_TOGGLE())

typedef uint16_t radio_packet_length_t;

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* BOARD_H_ */
