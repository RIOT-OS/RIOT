/*
 * Copyright (C) 2018 Eistec AB
 * Copyright (C) 2018 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_phynode-kw41z
 * @{
 *
 * @file
 * @brief       Board specific definitions for the USB-KW41Z
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author      José Alamos <jose.alamos@haw-hamburg.de>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* Set the FOPT bit to disable NMI so that we can use it as a GPIO pin for
 * the LED (PTB18) */
#define KINETIS_FOPT (0xff & ~(NV_FOPT_NMI_DIS_MASK))

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#define LED0_PIN            GPIO_PIN(PORT_B,  3)
#define LED0_MASK           (1 << 3)
#define LED0_ON             (GPIOB->PCOR = LED0_MASK)
#define LED0_OFF            (GPIOB->PSOR = LED0_MASK)
#define LED0_TOGGLE         (GPIOB->PTOR = LED0_MASK)

#define LED1_PIN            GPIO_PIN(PORT_B,  1)
#define LED1_MASK           (1 << 1)
#define LED1_ON             (GPIOB->PCOR = LED1_MASK)
#define LED1_OFF            (GPIOB->PSOR = LED1_MASK)
#define LED1_TOGGLE         (GPIOB->PTOR = LED1_MASK)

#define LED2_PIN            GPIO_PIN(PORT_B,  0)
#define LED2_MASK           (1 << 0)
#define LED2_ON             (GPIOB->PCOR = LED2_MASK)
#define LED2_OFF            (GPIOB->PSOR = LED2_MASK)
#define LED2_TOGGLE         (GPIOB->PTOR = LED2_MASK)

#define LED3_PIN            GPIO_PIN(PORT_B,  18)
#define LED3_MASK           (1 << 18)
#define LED3_ON             (GPIOB->PCOR = LED3_MASK)
#define LED3_OFF            (GPIOB->PSOR = LED3_MASK)
#define LED3_TOGGLE         (GPIOB->PTOR = LED3_MASK)
/** @} */

/**
 * @name    Button pin definitions
 * @{
 */
/* Pressing SW1 will short this pin to ground but there are no external pull
 * resistors, use internal pull-up on the pin */
/* BTN0 is mapped to SW1 */
#define BTN0_PIN            GPIO_PIN(PORT_C,  5)
#define BTN0_MODE           GPIO_IN_PU
/** @} */

/**
 * @name    Sensors voltage pin definitions (V_PERIPH)
 * @{
 */
#define V_PERIPH_PIN        GPIO_PIN(PORT_C, 19)
#define V_PERIPH_MASK       (1 << 19)
#define V_PERIPH_ON         (GPIOC->PSOR = V_PERIPH_MASK)
#define V_PERIPH_OFF        (GPIOC->PCOR = V_PERIPH_MASK)
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
/** @} */

/**
 * @name Define the interface for the CCS811 gas sensors
 * @{
 */
#define CCS811_PARAM_I2C_DEV        (I2C_DEV(0))
#define CCS811_PARAM_I2C_ADDR       (0x5A)
#define CCS811_PARAM_RESET_PIN      (GPIO_UNDEF)
#define CCS811_PARAM_WAKE_PIN       (GPIO_PIN(1, 2))
#define CCS811_PARAM_INT_PIN        (GPIO_PIN(1, 3))
#define CCS811_PARAM_INT_MODE       (CCS811_INT_NONE)
/** @} */

/**
 * @name Define the interface for the TCS37727 RGB light sensor
 * @{
 */
#define TCS37727_PARAM_I2C          (I2C_DEV(0))
#define TCS37727_PARAM_ADDR         (0x29)
/** @} */

/**
 * @name Define the interface for the MMA8X5X accelerometer
 * @{
 */
#define MMA8X5X_PARAM_I2C           (I2C_DEV(0))
#define MMA8X5X_PARAM_ADDR          (0x1D)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* BOARD_H */
