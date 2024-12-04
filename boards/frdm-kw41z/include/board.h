/*
 * Copyright (C) 2017 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_frdm-kw41z
 * @{
 *
 * @file
 * @brief       Board specific definitions for the FRDM-KW41Z
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "board_common.h"
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
#define LED1_PIN            GPIO_PIN(PORT_C,  1)
#define LED1_MASK           (1 << 1)
#define LED1_ON             (GPIOC->PCOR = LED1_MASK)
#define LED1_OFF            (GPIOC->PSOR = LED1_MASK)
#define LED1_TOGGLE         (GPIOC->PTOR = LED1_MASK)
#define LED2_PIN            GPIO_PIN(PORT_A, 19)
#define LED2_MASK           (1 << 19)
#define LED2_ON             (GPIOA->PCOR = LED2_MASK)
#define LED2_OFF            (GPIOA->PSOR = LED2_MASK)
#define LED2_TOGGLE         (GPIOA->PTOR = LED2_MASK)
#define LED3_PIN            GPIO_PIN(PORT_A, 18)
#define LED3_MASK           (1 << 18)
#define LED3_ON             (GPIOA->PCOR = LED3_MASK)
#define LED3_OFF            (GPIOA->PSOR = LED3_MASK)
#define LED3_TOGGLE         (GPIOA->PTOR = LED3_MASK)
/** @} */

/**
 * @name    Button pin definitions
 * @{
 */
/* SW3, SW4 will short these pins to ground when pushed but there are no
 * external pull resistors, use internal pull-ups on the pins */
/* BTN0 is mapped to SW3 */
#define BTN0_PIN            GPIO_PIN(PORT_C,  4)
#define BTN0_MODE           GPIO_IN_PU
/* BTN1 is mapped to SW4 */
#define BTN1_PIN            GPIO_PIN(PORT_C,  5)
#define BTN1_MODE           GPIO_IN_PU
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
 * @name    NOR flash hardware configuration
 * @{
 */
#define FRDM_NOR_SPI_DEV               SPI_DEV(0)
#define FRDM_NOR_SPI_CLK               SPI_CLK_5MHZ
#define FRDM_NOR_SPI_CS                SPI_HWCS(0) /**< Flash CS pin */
/** @} */

/**
 * @name    FXOS8700CQ 3-axis accelerometer and magnetometer bus configuration
 * @{
 */
#define FXOS8700_PARAM_I2C          I2C_DEV(0)
#define FXOS8700_PARAM_ADDR         0x1F
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* BOARD_H */
