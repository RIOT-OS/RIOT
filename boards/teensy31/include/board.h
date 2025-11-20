/*
 * SPDX-FileCopyrightText: 2017 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_teensy31
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Teensy3.1 & 3.2 board
 *
 * @author      Loïc Dauphin <loic.dauphin@inria.fr>
 */

#include "cpu.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Use the on board RTC 32kHz clock for LPTMR clocking. */
#undef LPTIMER_CLKSRC
/** @brief Clock source for the LPTMR module */
#define LPTIMER_CLKSRC LPTIMER_CLKSRC_ERCLK32K

/** Disable hardware watchdog, for debugging purposes, don't use this on production builds. */
#define DISABLE_WDOG    1

/**
 * @name   xtimer configuration
 * @{
 */
#define XTIMER_DEV                  (TIMER_PIT_DEV(0))
#define XTIMER_CHAN                 (0)
#define XTIMER_BACKOFF              (40)
#define XTIMER_ISR_BACKOFF          (40)
/** @} */

/**
 * @name   LED pin definitions and handlers
 * @{
 */
#define LED_PORT            PTC
#define LED0_BIT            (5)

#define LED0_PIN            GPIO_PIN(PORT_C, LED0_BIT)

#define LED0_ON             (LED_PORT->PSOR = (1 << LED0_BIT))
#define LED0_OFF            (LED_PORT->PCOR = (1 << LED0_BIT))
#define LED0_TOGGLE         (LED_PORT->PTOR = (1 << LED0_BIT))
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
