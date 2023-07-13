/*
 * Copyright (C) 2018 Koen Zandberg
 *               2021 Francisco Molina
 *               2023 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_st7796 ST7796 display driver
 * @ingroup     drivers_display
 *
 * @brief       Driver for the ST7796 display controller
 *
 * @{
 *
 * @file
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * The ST7796 is a generic display driver for small RGB displays. The driver
 * implemented here operates over SPI to communicate with the device.
 *
 * The device requires colors to be send in big endian RGB-565 format. The
 * @ref CONFIG_LCD_LE_MODE compile time option can switch this, but only use this
 * when strictly necessary. This option will slow down the driver as it
 * certainly can't use DMA anymore, every short has to be converted before
 * transfer.
 */


#ifndef ST7796_H
#define ST7796_H

#include "lcd.h"

#ifdef MODULE_DISP_DEV
#include "disp_dev.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    ST7796 display rotation modes
 * @{
 */
#define ST7796_ROTATION_VERT            0                   /**< Vertical mode */
#define ST7796_ROTATION_VERT_FLIP       LCD_MADCTL_MX | \
                                        LCD_MADCTL_MY       /**< Flipped vertical */
#define ST7796_ROTATION_HORZ            LCD_MADCTL_MV | \
                                        LCD_MADCTL_MX       /**< Horizontal mode */
#define ST7796_ROTATION_HORZ_FLIP       LCD_MADCTL_MV | \
                                        LCD_MADCTL_MY       /**< Horizontal flipped */
/** @} */

/**
 * @brief   Device descriptor for a st7796
 *
 */
typedef struct {
    lcd_t dev;                    /**< Pointer to the common lcd device */
} st7796_t;

/**
 * @brief   LCD device operations table
 */
extern const lcd_driver_t lcd_st7796_driver;

#ifdef __cplusplus
}
#endif
#endif /* ST7796_H */
/** @} */
