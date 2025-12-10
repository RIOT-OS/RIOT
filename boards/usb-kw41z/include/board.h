/*
 * SPDX-FileCopyrightText: 2018 Eistec AB
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_usb-kw41z
 * @{
 *
 * @file
 * @brief       Board specific definitions for the USB-KW41Z
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#include "cpu.h"
#include "board_common.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* PTB18 is configured for non-maskable interrupt at power on reset, but it is
 * also connected to the LED that is used by the JLink OpenSDA application as a
 * status indicator, which causes the KW41Z to become stuck in the NMI handler
 * at power on because the debugger interface is pulling the pin low to enable
 * the LED to show that it is connected. */
/* Set the FOPT bit to disable NMI so that we can use it as a GPIO pin for the LED */
#define KINETIS_FOPT (0xff & ~(NV_FOPT_NMI_DIS_MASK))
/**
 * @name    LED pin definitions and handlers
 * @{
 */
/* The LED pins on this board are connected to both the KW41Z and to the OpenSDA
 * K22F MCU, which may cause some issues between the two MCUs. The safest way to
 * use these LEDs is to cut the intentional shorts SH5, SH6 between the K22F and
 * the KW41Z. Cutting these traces will leave the KW41Z connected to the LEDs
 * but the K22F will be cut off. */
/* The connected pin on the K22F (PTC6) is configured as an input when running
 * the JLink OpenSDA application, so it should be reasonably safe to use this
 * LED from the KW41Z */
#define LED0_PIN            GPIO_PIN(PORT_B,  3)
#define LED0_MASK           (1 << 3)
#define LED0_ON             (GPIOB->PCOR = LED0_MASK)
#define LED0_OFF            (GPIOB->PSOR = LED0_MASK)
#define LED0_TOGGLE         (GPIOB->PTOR = LED0_MASK)
/* Enable this only if you have cut the intentional short SH6 which connects
 * the debugger pin to the LED and KW41Z PTB18. Otherwise we may end up with one
 * MCU driving the pin high and the other driving it low */
#if 0 /* user has cut SH6 */
/* The connected pin on the K22F (PTC7) is configured as a digital output with
 * high drive strength enabled in the JLink OpenSDA application, so this should
 * definitely not be configured as a digital output on the KW41Z unless the
 * corresponding short SH6 has been cut.*/
#define LED1_PIN            GPIO_PIN(PORT_B, 18)
#define LED1_MASK           (1 << 18)
#define LED1_ON             (GPIOB->PCOR = LED1_MASK)
#define LED1_OFF            (GPIOB->PSOR = LED1_MASK)
#define LED1_TOGGLE         (GPIOB->PTOR = LED1_MASK)
#endif /* user has cut SH6 */
/** @} */

/**
 * @name    Button pin definitions
 * @{
 */
/* Pressing SW1 will short this pin to ground but there are no external pull
 * resistors, use internal pull-up on the pin */
/* BTN0 is mapped to SW1 */
#define BTN0_PIN            GPIO_PIN(PORT_B,  2)
#define BTN0_MODE           GPIO_IN_PU
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
#define XTIMER_BACKOFF              (5)
#define XTIMER_ISR_BACKOFF          (5)
#define XTIMER_HZ                   (32768ul)
#endif
/** @} */

/**
 * @name    ztimer configuration
 * @{
 */
#define CONFIG_ZTIMER_USEC_TYPE     ZTIMER_TYPE_PERIPH_TIMER
#define CONFIG_ZTIMER_USEC_DEV      (TIMER_PIT_DEV(0))
#define CONFIG_ZTIMER_USEC_FREQ     (1000000LU)
#define CONFIG_ZTIMER_USEC_WIDTH    (32)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
