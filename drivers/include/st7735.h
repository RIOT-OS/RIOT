/*
 * Copyright (C) 2018 Koen Zandberg
 *               2021 Francisco Molina
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_st7735 ST7735 display driver
 * @ingroup     drivers_display
 *
 * @brief       Driver for the ST7735 display
 *
 * @{
 *
 * @file
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 * The ST7735 is a generic display driver for small RGB displays. The driver
 * implemented here operates over SPI to communicate with the device.
 *
 * The device requires colors to be send in big endian RGB-565 format. The
 * @ref CONFIG_LCD_LE_MODE compile time option can switch this, but only use this
 * when strictly necessary. This option will slow down the driver as it
 * certainly can't use DMA anymore, every short has to be converted before
 * transfer.
 */


#ifndef ST7735_H
#define ST7735_H

#include "lcd.h"

#ifdef MODULE_DISP_DEV
#include "disp_dev.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief ST7735 gvdd level.
 *
 * Default GVDD voltage of 4.8V. GVDD is reference level for the VCOM level and
 * the grayscale voltage level. GVDD should be ≦ (AVDD - 0.5) V .
 */
#ifndef CONFIG_ST7735_GVDD
#define CONFIG_ST7735_GVDD             4800
#endif

/**
 * @brief ST7735 VCOMH voltage level.
 *
 * Default VCOMH voltage of 4.25V. VCOMH represents the high level of VCOM AC
 * voltage. VCOM levels needs to be adjusted to match the capacitance and
 * performance specifications of the TFT panel to maximize contrast and minimize
 * flickering.
 */
#ifndef CONFIG_ST7735_VCOMH
#define CONFIG_ST7735_VCOMH            4250
#endif

/**
 * @brief ST7735 VCOML voltage level.
 *
 * Default VCOML voltage of -2V. VCOML represents the low level of VCOM AC
 * voltage. VCOM levels needs to be adjusted to match the capacitance and
 * performance specifications of the TFT panel to maximize contrast and minimize
 * flickering
 */
#ifndef CONFIG_ST7735_VCOML
#define CONFIG_ST7735_VCOML            -2000
#endif

/**
 * @name    ILI9341 display rotation modes
 * @{
 */
#define ST7735_ROTATION_VERT            0                   /**< Vertical mode */
#define ST7735_ROTATION_VERT_FLIP       LCD_MADCTL_MX | \
                                        LCD_MADCTL_MY       /**< Flipped vertical */
#define ST7735_ROTATION_HORZ            LCD_MADCTL_MV | \
                                        LCD_MADCTL_MX       /**< Horizontal mode */
#define ST7735_ROTATION_HORZ_FLIP       LCD_MADCTL_MV | \
                                        LCD_MADCTL_MY       /**< Horizontal flipped */
/** @} */

/**
 * @brief   Device descriptor for a st7735
 *
 */
typedef struct {
    lcd_t dev;                    /**< Pointer to the common lcd device */
} st7735_t;

/**
 * @brief   LCD device operations table
 */
extern const lcd_driver_t lcd_st7735_driver;

#ifdef __cplusplus
}
#endif
#endif /* ST7735_H */
/** @} */
