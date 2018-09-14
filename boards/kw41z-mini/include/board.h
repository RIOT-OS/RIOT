/*
 * Copyright (C) 2018 Tristan Bruns
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_kw41z-mini
 * @{
 *
 * @file
 * @brief       Board specific definitions for the kw41z-mini
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author      Maximilian Luenert <malu@uni-bremen.de>
 * @author      Tristan Bruns <tbruns@uni-bremen.de>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#define LED0_PIN            GPIO_PIN(PORT_B,  0)
#define LED0_MASK           (1 << 0)
#define LED0_ON             (GPIOB->PCOR = LED0_MASK)
#define LED0_OFF            (GPIOB->PSOR = LED0_MASK)
#define LED0_TOGGLE         (GPIOB->PTOR = LED0_MASK)
/** @} */

/**
 * @name    xtimer configuration
 * @{
 */
#if KINETIS_XTIMER_SOURCE_PIT
/* PIT xtimer configuration */
#define XTIMER_DEV                  (TIMER_PIT_DEV(0))
#define XTIMER_CHAN                 (0)
/* Default xtimer settings should work on the PIT */
#else
/* LPTMR xtimer configuration */
#define XTIMER_DEV                  (TIMER_LPTMR_DEV(0))
#define XTIMER_CHAN                 (0)
/* LPTMR is 16 bits wide and runs at 32768 Hz (clocked by the RTC) */
#define XTIMER_WIDTH                (16)
#define XTIMER_BACKOFF              (5)
#define XTIMER_ISR_BACKOFF          (5)
#define XTIMER_OVERHEAD             (4)
#define XTIMER_HZ                   (32768ul)
#endif
/** @} */

/**
 * @brief   Initialize board specific hardware, including clock, LEDs and standard I/O
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
