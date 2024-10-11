/*
 * Copyright (C) 2023 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_nrf5340dk-app
 * @{
 *
 * @file
 * @brief       Board configuration for the nRF5340DK-app board
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   System core clock speed, for all NRF53 Application core.
 */
#define CLOCK_CORECLOCK     MHZ(128)

/**
 * @name    Clock configuration
 *
 *
 * @{
 */

 /* CLOCK_HFCLKSRC_SRC_HFXO to use external 32MHz crystal
  *   CLOCK_HFCLKSRC_SRC_HFINT to use internal crystal */
#define CLOCK_HFCLK         (CLOCK_HFCLKSRC_SRC_HFXO) /**< HFCLK Source selection */

/* LFCLK Source clock selection:*/
/* - CLOCK_LFCLKSRC_SRC_LFRC: 32.768 kHz RC oscillator
 * - CLOCK_LFCLKSRC_SRC_LFXO: 32.768 kHz crystal oscillator
 * - CLOCK_LFCLKSRC_SRC_LFSYNT: 32.768 kHz synthesized from HFCLK*/
#define CLOCK_LFCLK         (CLOCK_LFCLKSRC_SRC_LFXO) /**< LFCLK Source */
/** @} */

/**
 * @name    LED pin configuration
 * @{
 */
#define LED0_PIN            GPIO_PIN(0, 28) /**< LED0 pin definition */
#define LED1_PIN            GPIO_PIN(0, 29) /**< LED1 pin definition */
#define LED2_PIN            GPIO_PIN(0, 30) /**< LED2 pin definition */
#define LED3_PIN            GPIO_PIN(0, 31) /**< LED3 pin definition */

#define LED0_MASK           (1 << 28)       /**< LED0 PORT bitmask */
#define LED1_MASK           (1 << 29)       /**< LED1 PORT bitmask */
#define LED2_MASK           (1 << 30)       /**< LED2 PORT bitmask */
#define LED3_MASK           (1 << 31)       /**< LED3 PORT bitmask */

#define LED_PORT            (NRF_P0_S) /**< Default LED PORT */

#define LED0_ON             (LED_PORT->OUTCLR = LED0_MASK) /**< LED0 ON macro */
#define LED0_OFF            (LED_PORT->OUTSET = LED0_MASK) /**< LED0 OFF macro */
#define LED0_TOGGLE         (LED_PORT->OUT   ^= LED0_MASK) /**< LED0 toggle macro */

#define LED1_ON             (LED_PORT->OUTCLR = LED1_MASK) /**< LED1 ON macro */
#define LED1_OFF            (LED_PORT->OUTSET = LED1_MASK) /**< LED1 OFF macro */
#define LED1_TOGGLE         (LED_PORT->OUT   ^= LED1_MASK) /**< LED1 toggle macro */

#define LED2_ON             (LED_PORT->OUTCLR = LED2_MASK) /**< LED2 ON macro */
#define LED2_OFF            (LED_PORT->OUTSET = LED2_MASK) /**< LED2 OFF macro */
#define LED2_TOGGLE         (LED_PORT->OUT   ^= LED2_MASK) /**< LED2 toggle macro */

#define LED3_ON             (LED_PORT->OUTCLR = LED3_MASK) /**< LED3 ON macro */
#define LED3_OFF            (LED_PORT->OUTSET = LED3_MASK) /**< LED3 OFF macro */
#define LED3_TOGGLE         (LED_PORT->OUT   ^= LED3_MASK) /**< LED3 toggle macro */
/** @} */

/**
 * @name    Button pin configuration
 * @{
 */
#define BTN0_PIN            GPIO_PIN(0, 23)  /**< BTN0 pin definition */
#define BTN0_MODE           GPIO_IN_PU       /**< BTN0 default mode */
#define BTN1_PIN            GPIO_PIN(0, 24)  /**< BTN1 pin definition */
#define BTN1_MODE           GPIO_IN_PU       /**< BTN1 default mode */
#define BTN2_PIN            GPIO_PIN(0, 8)   /**< BTN2 pin definition */
#define BTN2_MODE           GPIO_IN_PU       /**< BTN2 default mode */
#define BTN3_PIN            GPIO_PIN(0, 9)   /**< BTN3 pin definition */
#define BTN3_MODE           GPIO_IN_PU       /**< BTN3 default mode */
/** @} */

/**
 * @name MTD configuration
 * @{
 */
#define MTD_0       mtd_dev_get(0)

#define BOARD_QSPI_PIN_CS       GPIO_PIN(0, 18)     /**< SPI Flash Chip Select */
#define BOARD_QSPI_PIN_WP       GPIO_PIN(0, 15)     /**< SPI Flash Write Protect */
#define BOARD_QSPI_PIN_HOLD     GPIO_PIN(0, 16)     /**< SPI Flash Hold pin */
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* BOARD_H */
