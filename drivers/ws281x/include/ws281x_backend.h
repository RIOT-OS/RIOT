/*
 * SPDX-FileCopyrightText: 2019 Marian Buschsieweke
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_ws281x
 *
 * @{
 * @file
 * @brief       Backend configuration for WS2812/SK6812 RGB LEDs
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Properties of the ATmega backend.
 * @{
 */
#ifdef MODULE_WS281X_ATMEGA
#define WS281X_HAVE_INIT                    (1)
#endif
/** @} */

/**
 * @name    Properties of the ESP32 backend.
 * @{
 */
#ifdef MODULE_WS281X_ESP32
#define WS281X_HAVE_INIT                    (1)
#endif
/** @} */

/**
 * @name    Properties of the VT100 terminal backend.
 * @{
 */
#ifdef MODULE_WS281X_VT100
#define WS281X_HAVE_PREPARE_TRANSMISSION    (1)
#define WS281X_HAVE_END_TRANSMISSION        (1)
#endif
/** @} */

/**
 * @name    Properties of the timer_gpio_ll backend.
 * @{
 */
#ifdef MODULE_WS281X_TIMER_GPIO_LL
#define WS281X_HAVE_INIT                    (1)
#endif
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
