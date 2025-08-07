/*
 * SPDX-FileCopyrightText: 2020 Benjamin Valentin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_serpente
 * @brief       Support for the Serpente board.
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Serpente
 *              board
 *
 * @author      Benjamin Valentin <benpicco@googlemail.com>
 */

#include "cpu.h"
#include "periph_conf.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#define LED_PORT            PORT->Group[PA]

#define LED0_PIN            GPIO_PIN(PA, 22)
#define LED0_MASK           (1 << 22)
#define LED0_NAME           "LED(Red)"

#define LED0_OFF            (LED_PORT.OUTSET.reg = LED0_MASK)
#define LED0_ON             (LED_PORT.OUTCLR.reg = LED0_MASK)
#define LED0_TOGGLE         (LED_PORT.OUTTGL.reg = LED0_MASK)

#define LED1_PIN            GPIO_PIN(PA, 19)
#define LED1_MASK           (1 << 19)
#define LED1_NAME           "LED(Green)"

#define LED1_OFF            (LED_PORT.OUTSET.reg = LED1_MASK)
#define LED1_ON             (LED_PORT.OUTCLR.reg = LED1_MASK)
#define LED1_TOGGLE         (LED_PORT.OUTTGL.reg = LED1_MASK)

#define LED2_PIN            GPIO_PIN(PA, 23)
#define LED2_MASK           (1 << 23)
#define LED2_NAME           "LED(Blue)"

#define LED2_OFF            (LED_PORT.OUTSET.reg = LED2_MASK)
#define LED2_ON             (LED_PORT.OUTCLR.reg = LED2_MASK)
#define LED2_TOGGLE         (LED_PORT.OUTTGL.reg = LED2_MASK)
/** @} */

/**
 * @name Serpente NOR flash hardware configuration
 * @{
 */
#define SERPENTE_NOR_PAGE_SIZE          (256)
#define SERPENTE_NOR_PAGES_PER_SECTOR   (16)
#define SERPENTE_NOR_SECTOR_COUNT       (1024)
#define SERPENTE_NOR_FLAGS              (SPI_NOR_F_SECT_4K | SPI_NOR_F_SECT_32K)
#define SERPENTE_NOR_SPI_DEV            SPI_DEV(0)
#define SERPENTE_NOR_SPI_CLK            SPI_CLK_10MHZ
#define SERPENTE_NOR_SPI_CS             GPIO_PIN(PA, 15)
#define SERPENTE_NOR_SPI_MODE           SPI_MODE_3
/** @} */

/**
 * @name MTD configuration
 * @{
 */
#define MTD_0 mtd_dev_get(0)
/** @} */

/**
 * @name USB configuration
 * @{
 */
#define INTERNAL_PERIPHERAL_VID         (0x239A)
#define INTERNAL_PERIPHERAL_PID         (0x0057)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
