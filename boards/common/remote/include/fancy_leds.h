/*
 * SPDX-FileCopyrightText: 2014 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2015 Zolertia SL
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_common_remote
 * @{
 *
 * @file
 * @brief       LED extended functions
 *
 * @author      Antonio Lignan <alinan@zolertia.com>
 */

#include "board_common.h"

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @name Macro to create blinking and rainbows with the LEDs
 * @{
 */
/* Take LED_COLOR as argument, i.e LED0 */
#define LED_FADE(led)                         \
  volatile int i;                             \
  int k, j;                                   \
  LED_FADE_EXPAND(led)

#define LED_FADE_EXPAND(led)                  \
  for(k = 0; k < 800; ++k) {                  \
    j = k > 400 ? 800 - k : k;                \
    led##_ON;                                 \
    for(i = 0; i < j; ++i) {                  \
      __asm__("nop");                             \
    }                                         \
    led##_OFF;                                \
    for(i = 0; i < 400 - j; ++i) {            \
      __asm__("nop");                             \
    }                                         \
  }

#define LED_RAINBOW()                         \
  volatile int i;                             \
  int k,j;                                    \
  LED_FADE_EXPAND(LED3);                      \
  LED_FADE_EXPAND(LED0);                      \
  LED_FADE_EXPAND(LED4);                      \
  LED_FADE_EXPAND(LED2);                      \
  LED_FADE_EXPAND(LED1);
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif
/** @} */
