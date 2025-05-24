/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     boards_nrf52840dk
 * @{
 *
 * @file
 * @brief       Board specific configuration for the nRF52840 DK
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Sebastian Meiling <s@mlng.net>
 */

#include "board_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    LED pin configuration
 * @{
 */
#define LED0_PIN            GPIO_PIN(0, 13)
#define LED1_PIN            GPIO_PIN(0, 14)
#define LED2_PIN            GPIO_PIN(0, 15)
#define LED3_PIN            GPIO_PIN(0, 16)

#define LED_PORT            (NRF_P0)
#define LED0_MASK           (1 << 13)
#define LED1_MASK           (1 << 14)
#define LED2_MASK           (1 << 15)
#define LED3_MASK           (1 << 16)
#define LED_MASK            (LED0_MASK | LED1_MASK | LED2_MASK | LED3_MASK)

#define LED0_ON             (LED_PORT->OUTCLR = LED0_MASK)
#define LED0_OFF            (LED_PORT->OUTSET = LED0_MASK)
#define LED0_TOGGLE         (LED_PORT->OUT   ^= LED0_MASK)

#define LED1_ON             (LED_PORT->OUTCLR = LED1_MASK)
#define LED1_OFF            (LED_PORT->OUTSET = LED1_MASK)
#define LED1_TOGGLE         (LED_PORT->OUT   ^= LED1_MASK)

#define LED2_ON             (LED_PORT->OUTCLR = LED2_MASK)
#define LED2_OFF            (LED_PORT->OUTSET = LED2_MASK)
#define LED2_TOGGLE         (LED_PORT->OUT   ^= LED2_MASK)

#define LED3_ON             (LED_PORT->OUTCLR = LED3_MASK)
#define LED3_OFF            (LED_PORT->OUTSET = LED3_MASK)
#define LED3_TOGGLE         (LED_PORT->OUT   ^= LED3_MASK)
/** @} */

/**
 * @name SPI NOR flash hardware configuration
 *
 * A Macronix MX25R6435F is present on the board
 * @{
 */
#define NRF52840DK_NOR_PAGE_SIZE          (256)
#define NRF52840DK_NOR_PAGES_PER_SECTOR   (16)
#define NRF52840DK_NOR_SECTOR_COUNT       (2048)
#define NRF52840DK_NOR_FLAGS              (SPI_NOR_F_SECT_4K | SPI_NOR_F_SECT_32K)
#define NRF52840DK_NOR_SPI_DEV            SPI_DEV(1)
#define NRF52840DK_NOR_SPI_CLK            SPI_CLK_10MHZ
#define NRF52840DK_NOR_SPI_CS             GPIO_PIN(0, 17)
#define NRF52840DK_NOR_SPI_MODE           SPI_MODE_0
/** @} */

/** Default MTD device */
#define MTD_0 mtd_dev_get(0)

/**
 * @name    Button pin configuration
 * @{
 */
#define BTN0_PIN            GPIO_PIN(0, 11)
#define BTN0_MODE           GPIO_IN_PU
#define BTN1_PIN            GPIO_PIN(0, 12)
#define BTN1_MODE           GPIO_IN_PU
#define BTN2_PIN            GPIO_PIN(0, 24)
#define BTN2_MODE           GPIO_IN_PU
#define BTN3_PIN            GPIO_PIN(0, 25)
#define BTN3_MODE           GPIO_IN_PU
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
