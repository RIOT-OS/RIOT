/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_teensy31
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Teensy3.1 & 3.2 board
 *
 * @author      Loïc Dauphin <loic.dauphin@inria.fr>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "periph_conf.h"
#include "mtd.h"

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

/**
 * @brief Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
