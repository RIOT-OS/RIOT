/*
 * Copyright (C) 2019 Inria
 *               2019 Freie Universität Berlin
 *               2019 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_pinetime
 * @{
 *
 * @file
 * @brief       Board specific configuration for the PineTime
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "board_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name GPIO pin defines
 * @{
 */
#define LCD_RS                      GPIO_PIN(0, 18)
#define LCD_DET                     GPIO_PIN(0, 9)
#define LCD_CS                      GPIO_PIN(0, 25)
#define LCD_RESET                   GPIO_PIN(0, 26)

#define LCD_BACKLIGHT_LOW           GPIO_PIN(0, 14)
#define LCD_BACKLIGHT_MID           GPIO_PIN(0, 22)
#define LCD_BACKLIGHT_HIGH          GPIO_PIN(0, 23)

#define BUTTON0_ENABLE              GPIO_PIN(0, 13)
#define BUTTON0                     GPIO_PIN(0, 15)
#define VIBRATOR                    GPIO_PIN(0, 16)

#define VCC33                       GPIO_PIN(0, 24)
#define POWER_PRESENCE              GPIO_PIN(0, 19)
#define CHARGING_ACTIVE             GPIO_PIN(0, 12)
#define BATTERY_ADC                 NRF52_AIN7
/** @} */

/**
 * @name    Backlight control defines, default uses LCD_BACKLIGHT_LOW values
 * @{
 */
#ifndef BACKLIGHT_MASK
#define BACKLIGHT_MASK              (1 << 14)
#endif
#define BACKLIGHT_ON                (NRF_P0->OUTCLR = BACKLIGHT_MASK)
#define BACKLIGHT_OFF               (NRF_P0->OUTSET = BACKLIGHT_MASK)
/** @} */

/**
 * @name LCD configuration
 * @{
 */
#define ILI9341_PARAM_SPI          SPI_DEV(0)
#define ILI9341_PARAM_SPI_CLK      SPI_CLK_10MHZ
#define ILI9341_PARAM_SPI_MODE     SPI_MODE_3
#define ILI9341_PARAM_CS           LCD_CS
#define ILI9341_PARAM_DCX          LCD_RS
#define ILI9341_PARAM_RST          LCD_RESET
#define ILI9341_PARAM_RGB          1
#define ILI9341_PARAM_INVERTED     1
#define ILI9341_PARAM_NUM_LINES    240U
#define ILI9341_PARAM_ROTATION     ILI9341_ROTATION_HORZ_FLIP
/** @} */

/**
 * @name PineTime NOR flash hardware configuration
 * @{
 */
#define PINETIME_NOR_PAGE_SIZE          (256)
#define PINETIME_NOR_PAGES_PER_SECTOR   (16)
#define PINETIME_NOR_SECTOR_COUNT       (2048)
#define PINETIME_NOR_FLAGS              (SPI_NOR_F_SECT_4K | SPI_NOR_F_SECT_32K)
#define PINETIME_NOR_SPI_DEV            SPI_DEV(0)
#define PINETIME_NOR_SPI_CLK            SPI_CLK_10MHZ
#define PINETIME_NOR_SPI_CS             GPIO_PIN(0, 5)
#define PINETIME_NOR_SPI_MODE           SPI_MODE_3
/** @} */

/**
 * @name MTD configuration
 * @{
 */
#define MTD_0 mtd_dev_get(0)
/** @} */

/**
 * @name Touch screen configuration
 * @{
 */
#define CST816S_PARAM_I2C_DEV        I2C_DEV(0)
#define CST816S_PARAM_I2C_ADDR       (0x15)
#define CST816S_PARAM_IRQ            GPIO_PIN(0, 28)
#define CST816S_PARAM_IRQ_FLANK      GPIO_FALLING
#define CST816S_PARAM_RESET          GPIO_PIN(0, 10)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
