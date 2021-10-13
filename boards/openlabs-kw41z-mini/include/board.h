/*
 * Copyright (C) 2017 Thomas Stilwell <stilwellt@openlabs.co>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_openlabs-kw41z-mini
 * @{
 *
 * @file
 * @brief       Board specific definitions for openlabs-kw41z-mini
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author      Thomas Stilwell <stilwellt@openlabs.co>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*
 * NMI shares a pin with DAC output and ADC input. Holding the pin low during
 * reset will cause a hang unless NMI is disabled. It can be enabled in
 * applications where the pin is not held low during reset.
 */
#define KINETIS_FOPT 0xFB /* disable NMI (0xFF to enable) */

/**
 * @name   LED pin definitions and handlers
 * @{
 */
#define LED0_PIN           GPIO_PIN(PORT_B,  0)
#define LED0_MASK          (1 << 0)
#define LED0_ON            (GPIOB->PCOR = LED0_MASK)
#define LED0_OFF           (GPIOB->PSOR = LED0_MASK)
#define LED0_TOGGLE        (GPIOB->PTOR = LED0_MASK)
/** @} */

/**
 * @name    xtimer configuration
 * @{
 */
#if IS_ACTIVE(KINETIS_XTIMER_SOURCE_PIT)
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
#define XTIMER_BACKOFF              (16)
#define XTIMER_ISR_BACKOFF          (5)
#define XTIMER_HZ                   (32768ul)
#endif
/** @} */

/**
 * @def     PTB3_OUTPUT_OSC32K
 *
 * @brief   Enable LF oscillator output on PTB3 to aid debugging or calibration
 */
#ifndef PTB3_OUTPUT_OSC32K
#define PTB3_OUTPUT_OSC32K          (0)
#endif

/**
 * @def     PTB3_OUTPUT_OSCERCLK
 *
 * @brief   Enable HF oscillator output on PTB3 to aid debugging or calibration
 */
#ifndef PTB3_OUTPUT_OSCERCLK
#define PTB3_OUTPUT_OSCERCLK        (0)
#endif

/**
 * @brief Initialize board-specific hardware, including clock, LEDs, and stdio
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
