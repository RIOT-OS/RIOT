/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     boards_reel
 * @{
 *
 * @file
 * @brief       Board specific configuration for the Phytec 'reel board'
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    LED pin configuration
 * @{
 */
#define LED0_PIN                GPIO_PIN(0, 11)
#define LED1_PIN                GPIO_PIN(0, 12)
#define LED2_PIN                GPIO_PIN(1, 9)
#define LED3_PIN                GPIO_PIN(0, 13)

#define LED0_MASK               (1 << 11)
#define LED1_MASK               (1 << 12)
#define LED2_MASK               (1 << 9)
#define LED3_MASK               (1 << 13)
#define LED_MASK_P0             (LED0_MASK | LED1_MASK | LED3_MASK)
#define LED_MASK_P1             (LED2_MASK)

#define LED0_ON                 (NRF_P0->OUTCLR = LED0_MASK)
#define LED0_OFF                (NRF_P0->OUTSET = LED0_MASK)
#define LED0_TOGGLE             (NRF_P0->OUT   ^= LED0_MASK)

#define LED1_ON                 (NRF_P0->OUTCLR = LED1_MASK)
#define LED1_OFF                (NRF_P0->OUTSET = LED1_MASK)
#define LED1_TOGGLE             (NRF_P0->OUT   ^= LED1_MASK)

#define LED2_ON                 (NRF_P1->OUTCLR = LED2_MASK)
#define LED2_OFF                (NRF_P1->OUTSET = LED2_MASK)
#define LED2_TOGGLE             (NRF_P1->OUT   ^= LED2_MASK)

#define LED3_ON                 (NRF_P0->OUTCLR = LED3_MASK)
#define LED3_OFF                (NRF_P0->OUTSET = LED3_MASK)
#define LED3_TOGGLE             (NRF_P0->OUT   ^= LED3_MASK)
/** @} */

/**
 * @name    Button pin configuration
 * @{
 */
#define BTN0_PIN                GPIO_PIN(0, 7)
#define BTN0_MODE               GPIO_IN_PU
/** @} */

/**
 * @name    Electronic paper display configuration (SSD1673fb)
 * @{
 */
#define SSD1673_SPI             SPI_DEV(0)
#define SSD1673_CS              GPIO_PIN(0, 17)
#define SSD1673_BUSY            GPIO_PIN(0, 14)
#define SSD1673_RESET           GPIO_PIN(0, 15)
#define SSD1673_DC_INPUT        GPIO_PIN(0, 16)
/** @} */

/**
 * @name    Humidity and temperature sensor configuration (HDC1010)
 * @{
 */
#define HDC1010_PARAM_I2C       I2C_DEV(0)
#define HDC1010_PARAM_ADDR      (0x43)
#define HDC1010_PIN_DRDY        GPIO_PIN(0, 22)
/** @} */

/**
 * @name    Digital accelerometer configuration (MMA8652FC)
 * @{
 */
#define MMA8X5X_PARAM_I2C       I2C_DEV(0)
#define MMA8X5X_PARAM_ADDR      (0x1d)
#define MMA8X5X_PARAM_PIN_INT1  GPIO_PIN(0, 24)
#define MMA8X5X_PARAM_PIN_INT2  GPIO_PIN(0, 25)
/** @} */

/**
 * @name    Digital proximity and ambient light sensor configuration (ADPS9960)
 * @{
 */
#define ADPS9960_PARAM_I2C      I2C_DEV(0)
#define ADPS9960_PARAM_ADDR     (0x29)
#define ADPS9960_PARAM_PIN_INT  GPIO_PIN(0, 23)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
