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
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
